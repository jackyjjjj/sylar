#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <functional>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <boost/lexical_cast.hpp>
#include "/usr/local/include/yaml-cpp/yaml.h"
#include <yaml-cpp/yaml.h>
#include "../include/log.h"

using namespace std;

namespace jyl
{
    class ConfigVarBase
    {
    public:
        typedef shared_ptr<ConfigVarBase> ptr;
        ConfigVarBase(const string &name, const string &description)
            : m_name(name), m_description(description)
        {
            transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
        }
        virtual ~ConfigVarBase() {}

        const string &getName() const { return m_name; }
        const string &getDescription() const { return m_description; }

        virtual string toString() = 0;
        virtual bool fromString(const string &val) = 0;
        virtual string getTypeName() const = 0;

    protected:
        string m_name;
        string m_description;
    };

    // transform from type F to type T
    template <class F, class T>
    class LexicalCast
    {
    public:
        T operator()(const F &val)
        {
            return boost::lexical_cast<T>(val);
        }
    };

    template <class T>
    class LexicalCast<string, vector<T>>
    {
    public:
        vector<T> operator()(const string &val)
        {
            YAML::Node node = YAML::Load(val);
            vector<T> vec;
            stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<string, T>()(ss.str()));
            }
            return vec;
        }
    };

    template <class T>
    class LexicalCast<vector<T>, string>
    {
    public:
        string operator()(const vector<T> &val)
        {
            YAML::Node node;
            for (auto &v : val)
            {
                node.push_back(YAML::Load(LexicalCast<T, string>()(v)));
            }
            stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    template <class T>
    class LexicalCast<string, list<T>>
    {
    public:
        list<T> operator()(const string &val)
        {
            YAML::Node node = YAML::Load(val);
            list<T> vec;
            stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.push_back(LexicalCast<string, T>()(ss.str()));
            }
            return vec;
        }
    };

    template <class T>
    class LexicalCast<list<T>, string>
    {
    public:
        string operator()(const list<T> &val)
        {
            YAML::Node node;
            for (auto &v : val)
            {
                node.push_back(YAML::Load(LexicalCast<T, string>()(v)));
            }
            stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    template <class T>
    class LexicalCast<string, set<T>>
    {
    public:
        set<T> operator()(const string &val)
        {
            YAML::Node node = YAML::Load(val);
            set<T> vec;
            stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<string, T>()(ss.str()));
            }
            return vec;
        }
    };

    template <class T>
    class LexicalCast<set<T>, string>
    {
    public:
        string operator()(const set<T> &val)
        {
            YAML::Node node;
            for (auto &v : val)
            {
                node.push_back(YAML::Load(LexicalCast<T, string>()(v)));
            }
            stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    template <class T>
    class LexicalCast<string, unordered_set<T>>
    {
    public:
        unordered_set<T> operator()(const string &val)
        {
            YAML::Node node = YAML::Load(val);
            unordered_set<T> vec;
            stringstream ss;
            for (size_t i = 0; i < node.size(); ++i)
            {
                ss.str("");
                ss << node[i];
                vec.insert(LexicalCast<string, T>()(ss.str()));
            }
            return vec;
        }
    };

    template <class T>
    class LexicalCast<unordered_set<T>, string>
    {
    public:
        string operator()(const unordered_set<T> &val)
        {
            YAML::Node node;
            for (auto &v : val)
            {
                node.push_back(YAML::Load(LexicalCast<T, string>()(v)));
            }
            stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    template <class T>
    class LexicalCast<string, map<string, T>>
    {
    public:
        map<string, T> operator()(const string &val)
        {
            YAML::Node node = YAML::Load(val);
            map<string, T> vec;
            stringstream ss;
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ss.str("");
                ss << it->second;
                vec.insert(make_pair(it->first.Scalar(), LexicalCast<string, T>()(ss.str())));
            }
            return vec;
        }
    };

    template <class T>
    class LexicalCast<map<string, T>, string>
    {
    public:
        string operator()(const map<string, T> &val)
        {
            YAML::Node node;
            for (auto &v : val)
            {
                node[v.first] = YAML::Load(LexicalCast<T, string>()(v.second));
            }
            stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    template <class T>
    class LexicalCast<string, unordered_map<string, T>>
    {
    public:
        unordered_map<string, T> operator()(const string &val)
        {
            YAML::Node node = YAML::Load(val);
            unordered_map<string, T> vec;
            stringstream ss;
            for (auto it = node.begin(); it != node.end(); ++it)
            {
                ss.str("");
                ss << it->second;
                vec.insert(make_pair(it->first.Scalar(), LexicalCast<string, T>()(ss.str())));
            }
            return vec;
        }
    };

    template <class T>
    class LexicalCast<unordered_map<string, T>, string>
    {
    public:
        string operator()(const unordered_map<string, T> &val)
        {
            YAML::Node node;
            for (auto &v : val)
            {
                node[v.first] = YAML::Load(LexicalCast<T, string>()(v.second));
            }
            stringstream ss;
            ss << node;
            return ss.str();
        }
    };

    // FromStr T operator() (const string&)
    // ToStr string operator()(const T&)
    // 配置信息的类，一条配置信息就是一个对象
    template <class T, class FromStr = LexicalCast<string, T>, class ToStr = LexicalCast<T, string>>
    class ConfigVar : public ConfigVarBase
    {
    public:
        typedef std::shared_ptr<ConfigVar> ptr;
        typedef function<void(const &old_val, const T &new_val)> on_change_cb;

        ConfigVar(const string &name, const T &default_val, const string &description)
            : ConfigVarBase(name, description), m_val(default_val) {}

        string toString() override
        {
            try
            {
                // return boost::lexical_cast<string>(m_val);
                return ToStr()(m_val);
            }
            catch (exception &e)
            {
                /*log 日志*/
            }
            return "";
        }
        bool fromString(const string &val) override
        {
            try
            {
                // m_val = boost::lexical_cast<T>(val);
                setValue(FromStr()(val));
                return true;
            }
            catch (exception &e)
            {
                /*log 日志*/
            }
            return false;
        }
        const T getValue() const { return m_val; }
        void setValue(const T &val)
        {
            // 这里进行比较需要对自定义的类的 == 进行重载
            if (val == m_val)
            {
                return;
            }
            for (auto &i : m_cbs)
            {
                i.second(m_val, val);
            }
            m_val = val;
        }
        string getTypeName() const override { return typeid(T).name(); }
        void addListener(uint64_t key, const on_change_cb &cb) { m_cbs[key] = cb; }
        void delListener(uint64_t key) { m_cbs.erase(key); }
        on_change_cb getListener(uint64_t key)
        {
            auto it = m_cbs.find(key);
            return it == nullptr ? nullptr : it->second;
        }
        void clearListener() { m_cbs.clear(); }

    private:
        T m_val;

        // 变更回调函数，根据key查找删除回调函数，key要求唯一，一般可以用hash
        map<uint64_t, on_change_cb> m_cbs;
    };

    class Config
    {
    public:
        typedef map<string, ConfigVarBase::ptr> ConfigVarMap;
        static void LoadFromYaml(const YAML::Node &node);
        /*typename用于指示这里返回的是一个COnfigVar<T> 类型的ptr*/
        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const string &name, const T &default_val,
                                                 const string &description = "")
        {
            auto it = s_datas.find(name);
            if (it != s_datas.end())
            {
                auto tmp = dynamic_pointer_cast<ConfigVar<T>>(it->second);
                if (tmp)
                {
                    JYL_LOG_INFO(JYL_LOG_ROOT()) << "Lookup name=" << name << "exsits";
                }
                else
                {
                    JYL_LOG_INFO(JYL_LOG_ROOT()) << "Lookup name=" << name << " exsits,but type not match,type:"
                                                 << typeid(T).name() << " real type is " << it->second->getTypeName()
                                                 << ". the value is " << it->second->toString();
                    return nullptr;
                }
            }
            auto tmp = Lookup<T>(name);
            if (tmp)
            {
                /*log日志*/
            }
            if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz./-0123456789") != string::npos)
            {
                /*log*/
                // throw invalid_argument(name);
            }
            typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_val, description));
            if (s_datas.find(name) == s_datas.end())
            {
                // 没找到就插入
                s_datas.insert(make_pair(name, v));
            }
            else
            {
                s_datas[name] = v;
            }
            return v;
        }

        template <class T>
        static typename ConfigVar<T>::ptr Lookup(const string &name)
        {
            auto it = s_datas.find(name);
            if (it == s_datas.end())
            {
                return nullptr;
            }
            return dynamic_pointer_cast<ConfigVar<T>>(it->second);
        }
        // static void LoadFromYaml(const YAML::Node &node);
        static ConfigVarBase::ptr lookupBase(const string &name);

    private:
        static ConfigVarMap s_datas; // 保存的配置信息
    };

}