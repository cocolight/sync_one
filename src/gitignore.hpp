#pragma once
#include <algorithm>
#include <string>
#include <vector>

class GitIgnore {
 public:
  /**
   * @brief GitIgnore构造函数，使用给定的规则列表初始化GitIgnore对象
   *
   * @param rules 包含Git忽略规则的字符串向量
   */
  explicit GitIgnore(const std::vector<std::string>& rules) {
    // 遍历所有规则并逐个添加到GitIgnore对象中
    for (auto& r : rules) addRule(r);
  }
  /**
   * @brief 检查指定路径是否被忽略
   *
   * 该函数通过遍历规则列表来判断给定路径是否应该被忽略。
   * 规则分为两种类型：忽略规则（true）和非忽略规则（false），
   * 按照规则顺序处理，后面的规则可以覆盖前面的规则结果。
   *
   * @param path 待检查的路径字符串
   * @return true 如果路径被忽略则返回true，否则返回false
   */
  bool isIgnored(const std::string& path) const {
    std::string p = path;
    // 将路径中的反斜杠替换为正斜杠，统一路径分隔符格式
    std::replace(p.begin(), p.end(), '\\', '/');
    bool ignored = false;
    for (auto& rule : rules_) {
      // 遍历所有规则，如果规则类型为忽略且路径匹配，则设置为忽略状态
      // 如果规则类型为非忽略且路径匹配，则取消忽略状态
      if (rule.first && match(p, rule.second))
        ignored = true;
      else if (!rule.first && match(p, rule.second))
        ignored = false;
    }
    return ignored;
  }

 private:
  /**
   * 添加规则到规则列表中
   *
   * 该函数处理输入的规则字符串，去除尾部空白字符，过滤空行和注释行，
   * 支持否定规则（以!开头），并将处理后的规则添加到内部规则容器中
   *
   * @param rule 输入的规则字符串，可能包含尾部空白字符、注释或否定标识
   * @return 无返回值
   */
  void addRule(const std::string& rule) {
    // 去除字符串尾部的空白字符（空格、回车、换行、制表符）
    std::string r = rule;
    r.erase(r.find_last_not_of(" \r\n\t") + 1);
    if (r.empty() || r[0] == '#') return;
    // 解析规则的否定标识并提取实际规则内容
    bool neg = false;
    if (r[0] == '!') {
      neg = true;
      r = r.substr(1);
    }
    rules_.emplace_back(neg, r);
  }
  /**
   * @brief 检查路径字符串中是否包含指定的模式字符串
   *
   * 该函数通过在路径字符串中查找模式字符串来判断是否存在匹配
   *
   * @param path 要搜索的目标路径字符串
   * @param pattern 要查找的模式字符串
   * @return bool 如果在path中找到pattern则返回true，否则返回false
   */
  bool match(const std::string& path, const std::string& pattern) const {
    return path.find(pattern) != std::string::npos;
  }
  std::vector<std::pair<bool, std::string>> rules_;
};