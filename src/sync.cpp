#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "gitignore.hpp"

namespace fs = std::filesystem;

static GitIgnore* g_ignore = nullptr;

/**
 * @brief 检查指定路径是否被忽略
 *
 * 该函数检查给定的文件路径是否在忽略列表中。如果全局忽略开关未启用，
 * 则直接返回false；否则调用全局忽略对象的isIgnored方法进行检查。
 *
 * @param p 要检查的文件系统路径对象
 * @return bool 如果路径被忽略返回true，否则返回false
 */
bool isIgnored(const fs::path& p) {
  if (!g_ignore) return false;
  return g_ignore->isIgnored(p.string());
}

/**
 * @brief 同步文件：将源文件复制到目标位置
 *
 * 该函数将源文件复制到目标路径，如果目标文件已存在则会覆盖或更新
 *
 * @param src 源文件路径
 * @param dst 目标文件路径
 * @return 无返回值
 */
void syncFile(const fs::path& src, const fs::path& dst) {
  try {
    // 执行文件复制操作，覆盖或更新已存在的目标文件
    fs::copy_file(src, dst,
                  fs::copy_options::overwrite_existing |
                      fs::copy_options::update_existing);
    std::cout << "[COPY] " << src << " -> " << dst << '\n';
  } catch (const std::exception& e) {
    // 输出复制操作失败的错误信息
    std::cerr << "[ERROR] " << e.what() << '\n';
  }
}

/**
 * @brief 收集需要删除的文件路径
 *
 * 遍历目标根目录下的所有文件和子目录，找出在目标目录中存在但在源目录中不存在的文件，
 * 将这些文件路径添加到待删除列表中。
 *
 * @param dstRoot 目标根目录路径
 * @param srcRoot 源根目录路径
 * @param toRemove 用于存储需要删除的文件路径的向量引用
 */
void collectRemovals(const fs::path& dstRoot, const fs::path& srcRoot,
                     std::vector<fs::path>& toRemove) {
  // 遍历目标目录下的所有文件和子目录
  for (auto& entry : fs::recursive_directory_iterator(dstRoot)) {
    // 计算当前条目相对于目标根目录的相对路径
    fs::path rel = fs::relative(entry.path(), dstRoot);
    // 构造对应的源目录路径
    fs::path srcFile = srcRoot / rel;
    // 如果当前路径被忽略，则跳过处理
    if (isIgnored(rel.string())) continue;
    // 如果源目录中不存在对应的文件，则将目标目录中的文件添加到待删除列表
    if (!fs::exists(srcFile)) toRemove.push_back(entry.path());
  }
}

/**
 * @brief 镜像同步函数，将源目录的内容同步到目标目录
 *
 * 该函数实现目录镜像功能，包括：
 * 1. 确保目标根目录存在
 * 2. 删除目标目录中源目录不存在的文件和目录
 * 3. 复制源目录中的新增或更新的文件和目录到目标目录
 *
 * @param src 源目录路径
 * @param dst 目标目录路径
 */
void mirror(const fs::path& src, const fs::path& dst) {
  // 1. 确保目标根存在
  fs::create_directories(dst);

  // 2. 先收集要删除的条目（逆序保证先文件后目录）
  std::vector<fs::path> toRemove;
  collectRemovals(dst, src, toRemove);
  std::sort(toRemove.begin(), toRemove.end(),
            [](auto& a, auto& b) { return a.string() > b.string(); });
  for (auto& p : toRemove) {
    if (fs::is_directory(p)) {
      fs::remove(p);
      std::cout << "[DEL D] " << p << '\n';
    } else {
      fs::remove(p);
      std::cout << "[DEL F] " << p << '\n';
    }
  }

  // 3. 遍历源，拷贝新增/更新
  for (auto& entry : fs::recursive_directory_iterator(src)) {
    const auto& srcPath = entry.path();
    fs::path rel = fs::relative(srcPath, src);
    if (isIgnored(rel.string())) continue;
    fs::path dstPath = dst / rel;

    if (fs::is_directory(srcPath)) {
      fs::create_directories(dstPath);
    } else {
      if (!fs::exists(dstPath) ||
          fs::last_write_time(srcPath) != fs::last_write_time(dstPath) ||
          fs::file_size(srcPath) != fs::file_size(dstPath)) {
        fs::create_directories(dstPath.parent_path());
        syncFile(srcPath, dstPath);
        // 保留时间戳
        fs::last_write_time(dstPath, fs::last_write_time(srcPath));
      }
    }
  }
}

/**
 * @brief 主函数，执行文件同步操作
 *
 * @param argc 命令行参数个数
 * @param argv 命令行参数数组，格式为：程序名 <源路径> <目标路径> [gitignore文件路径]
 * @return int 返回状态码，0表示成功，1表示失败
 */
int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <src> <dst> [gitignore]\n";
    return 1;
  }
  fs::path src = argv[1], dst = argv[2];

  // 加载 ignore 规则
  std::vector<std::string> rules;
  if (argc == 4 && fs::exists(argv[3])) {
    std::ifstream ig(argv[3]);
    std::string line;
    while (std::getline(ig, line)) rules.push_back(line);
  }
  GitIgnore ignore(rules);
  g_ignore = &ignore;

  mirror(src, dst);
  std::cout << "Sync finished.\n";
  return 0;
}