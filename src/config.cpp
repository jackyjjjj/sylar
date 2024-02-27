#include "../include/config.h"
#include <list>
#include "../include/log.h"
#include <string>
namespace jyl
{

    Config::ConfigVarMap Config::s_datas;

    ConfigVarBase::ptr Config::lookupBase(const string &name)
    {

        auto it = s_datas.find(name);
        return it == s_datas.end() ? nullptr : it->second;
    }

    static void ListAllMembers(const string &prefix, const YAML::Node &node, list<pair<string, YAML::Node>> &output)
    {
        if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789") != string::npos)
        {
            {
                jyl::Logger::ptr logger(new jyl::Logger);
                JYL_LOG_LEVEL(logger, LogLevel::Level::DEBUG) << "config invalid name" << prefix << " = " << node;
            }
        }
        output.push_back(make_pair(prefix, node));
        if (node.IsMap())
        {
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ListAllMembers(prefix.empty() ? it->first.Scalar() : prefix + "." + it->first.Scalar(),
                               it->second, output);
            }
        }
    }

    void Config::LoadFromYaml(const YAML::Node &node)
    {

        list<pair<string, YAML::Node>> all_nodes;
        ListAllMembers("", node, all_nodes);
        for (auto &i : all_nodes)
        {
            string key = i.first;
            if (key.empty())
            {
                continue;
            }
            transform(key.begin(), key.end(), key.begin(), ::tolower);
            ConfigVarBase::ptr var = jyl::Config::lookupBase(key);
            if (var)
            {
                if (i.second.IsScalar())
                {
                    var->fromString(i.second.Scalar());
                }
                else
                {
                    stringstream ss;
                    ss << i.second;
                    var->fromString(ss.str());
                }
            }
        }
    }
}