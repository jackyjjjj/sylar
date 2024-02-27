#include "../include/log.h"
#include <map>
#include <functional>

namespace jyl
{
    const char *LogLevel::toString(LogLevel::Level level)
    {
        switch (level)
        {
#define XX(name)         \
    case LogLevel::name: \
        return #name;
            break;
            XX(DEBUG)
            XX(INFO)
            XX(WARN)
            XX(ERROR)
            XX(FATAL)
#undef XX
        default:
            return "UNKNOWN";
        }
    }

    class MessageFormatItem : public LogFormatter::FormatItem
    {
    public:
        MessageFormatItem(const string &fmt = "m") {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getContent();
        }
    };
    class LevelFormatItem : public LogFormatter::FormatItem
    {
    public:
        LevelFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << LogLevel::toString(level);
        }
    };
    class ElapseFormatItem : public LogFormatter::FormatItem
    {
    public:
        ElapseFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << logger->getName();
        }
    };
    class NameFormatItem : public LogFormatter::FormatItem
    {
    public:
        NameFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLogger()->getName();
        }
    };
    class ThreadIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        ThreadIdFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getThreadID();
        }
    };
    class FiberIdFormatItem : public LogFormatter::FormatItem
    {
    public:
        FiberIdFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFiberID();
        }
    };
    class DateTimeFormatItem : public LogFormatter::FormatItem
    {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
            : m_format(format)
        {
            if (m_format.empty())
            {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }

    private:
        string m_format;
    };
    class LineFormatItem : public LogFormatter::FormatItem
    {
    public:
        LineFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getLine();
        }
    };
    class NewLineFormatItem : public LogFormatter::FormatItem
    {
    public:
        NewLineFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << endl;
        }
    };
    class FileFormatItem : public LogFormatter::FormatItem
    {
    public:
        FileFormatItem(const string &fmt) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << event->getFile();
        }
    };
    class StringFormatItem : public LogFormatter::FormatItem
    {
    public:
        StringFormatItem(const string &str = "") : FormatItem(str), m_string(str) {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << m_string;
        }

    private:
        string m_string;
    };

    class TabFormatItem : public LogFormatter::FormatItem
    {
    public:
        TabFormatItem(const string &str = "") {}
        void format(ostream &os, shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override
        {
            os << '\t';
        }
    };

    Logger::Logger(const string &name)
        : m_name(name), m_level(LogLevel::DEBUG)
    {
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T%f:%l%T%m%n"));
    }

    LogEvent::LogEvent(shared_ptr<Logger> logger, LogLevel::Level level, const char *file, int32_t m_line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint32_t time)
        : m_file(file), m_line(m_line), m_elapsed(elapse), m_threadID(thread_id), m_fiberID(fiber_id), m_time(time), m_logger(logger), m_level(level)
    {
        // cout << "logevent" << endl;
    }
    LogEvent::~LogEvent()
    {
    }
    void LogEvent::format(const char *fmt, ...)
    {
        va_list al;
        va_start(al, fmt);
        format(fmt, al);
        va_end(al);
    }
    void LogEvent::format(const char *fmt, va_list al)
    {
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if (len != -1)
        {
            m_ss << string(buf, len);
            free(buf);
        }
    }
    LogEventWrap::LogEventWrap(LogEvent::ptr event) : m_event(event)
    {
    }
    LogEventWrap::~LogEventWrap()
    {

        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }
    // LogEvent::ptr LogEventWrap::getEvent() const { return m_event; }

    stringstream &LogEventWrap::getSS() { return m_event->getSS(); }

    void Logger::addAppender(LogAppender::ptr appender)
    {
        if (!appender->getFormatter())
        {
            appender->setFormatter(m_formatter);
        }
        m_appenderList.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = m_appenderList.begin(); it != m_appenderList.end(); ++it)
        {
            if (*it == appender)
            {
                m_appenderList.erase(it);
                break;
            }
        }
    }

    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        auto self = shared_from_this();
        if (level >= m_level)
        {
            if (!m_appenderList.empty())
            {
                for (auto &i : m_appenderList)
                {
                    i->log(self, level, event);
                }
            }
            else if (m_root)
            {
                m_root->log(level, event);
            }
        }
    }
    void Logger::debug(LogEvent::ptr event) { log(LogLevel::DEBUG, event); }
    void Logger::info(LogEvent::ptr event) { log(LogLevel::INFO, event); }
    void Logger::warn(LogEvent::ptr event) { log(LogLevel::WARN, event); }
    void Logger::error(LogEvent::ptr event) { log(LogLevel::ERROR, event); }
    void Logger::fatal(LogEvent::ptr event) { log(LogLevel::FATAL, event); }

    FileLogAppender::FileLogAppender(const string &filename) : m_filename(filename)
    {
    }
    void FileLogAppender::log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            reopen();
            m_fileStream << m_formatter->format(logger, level, event) << endl;
            m_fileStream.flush();
        }
    }
    bool FileLogAppender::reopen()
    {
        if (m_fileStream.is_open())
        {
            m_fileStream.close();
        }
        m_fileStream.open(m_filename);
        return !m_fileStream.fail();
    }

    void StdoutLogAppender::log(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            cout << m_formatter->format(logger, level, event) << endl;
        }
    }

    LogFormatter::LogFormatter(const string &pattern) : m_pattern(pattern) { init(); }

    void LogFormatter::init()
    {
        vector<tuple<string, string, int>> vec;
        string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] != '%')
            {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;
            string str;
            string fmt;
            while (n < m_pattern.size())
            {
                if (!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}'))
                {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }

                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; // 解析格式
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                }
                if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if (n == m_pattern.size())
                {
                    if (str.empty())
                    {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }
            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(make_tuple(nstr, string(), 0));
                    nstr.clear();
                }
                vec.push_back(make_tuple(str, fmt, 1));
                i = n - 1;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if (!nstr.empty())
        {
            vec.push_back(make_tuple(nstr, string(), 0));
        }

        /*
            %m 消息体
            %p level
            %r 启动后的时间
            %c 日志名称
            %n 回车换行
            %d 时间
            %f 文件名
            %l 行号
        */
        static map<string, function<LogFormatter::FormatItem::ptr(const string &str)>> s_format_items = {
#define XX(str, C)                                                                       \
    {                                                                                    \
        str, [](const string &fmt) { return LogFormatter::FormatItem::ptr(new C(fmt)); } \
    }
            XX("m", MessageFormatItem),
            XX("p", LevelFormatItem),
            XX("r", ElapseFormatItem),
            XX("c", NameFormatItem),
            XX("t", ThreadIdFormatItem),
            XX("n", NewLineFormatItem),
            XX("d", DateTimeFormatItem),
            XX("f", FileFormatItem),
            XX("l", LineFormatItem),
            XX("T", TabFormatItem),
            XX("F", FiberIdFormatItem)
#undef XX
        };
        for (auto &i : vec)
        {
            if (get<2>(i) == 0)
            {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(get<0>(i))));
            }
            else
            {
                auto it = s_format_items.find(get<0>(i));
                if (it == s_format_items.end())
                {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + get<0>(i) + ">>")));
                    m_error = true;
                }
                else
                {
                    m_items.push_back(it->second(get<1>(i)));
                }
            }
            // std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
        }
    }

    string LogFormatter::format(shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &i : m_items)
        {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    LoggerManager::LoggerManager()
    {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
        m_loggers[m_root->m_name] = m_root;
        init();
    }

    Logger::ptr LoggerManager::getLogger(const string &name)
    {
        auto it = m_loggers.find(name);
        if (it != m_loggers.end())
        {
            return it->second;
        }
        Logger::ptr logger(new Logger(name));
        logger->m_root = m_root;
        m_loggers[name] = logger;
        return logger;
    }
    struct LogAppendderDefine
    {
        int type=0;/*1.file  2.stdout*/
        LogLevel::Level level;
        string format;
        string file;

        bool operator==(const LogAppendderDefine &other) const
        {
            return type == other.type && level == other.level && format == other.format && file == other.file;
        }
    };
    struct LogDefine
    {
        string name;
        LogLevel::Level level = LogLevel::Level::UNKNOWN;
        string format;

        vector<LogAppenderDefine> appenders;
        bool operator==(const LogDefine &other) const
        {
            return name == other.name && level == other.level &&
                format == other.format && appenders == other.appenders;
        }
    };

    void LoggerManager::init()
    {

    }
}