#ifndef STATICLOGGER_H
#define STATIFLOGGER_H

#include <string>

class Logger
{
    private:
    static Logger* m_self;
    std::string m_logString;

    ~Logger()
    {

    };
    public:
    Logger()
    {
        m_logString = "";
    };

    static void Log(float _f)
    {
        if(!m_self)
        {
            m_self = new Logger();
        }

        m_self->m_logString += std::to_string(_f) + ", ";
    };
    
    static void Log(std::string _s)
    {
        if(!m_self)
        {
            m_self = new Logger();
        }

        m_self->m_logString += _s + ", ";
    };

    static void Flush()
    {
        if(!m_self)
        {
            m_self = new Logger();
        }

        printf("%s", m_self->m_logString);
        m_self->m_logString = "";
    };
};

#endif