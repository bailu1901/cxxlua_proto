#ifndef __BASE_NET_SOCKET_NET_COMMON_H__
#define __BASE_NET_SOCKET_NET_COMMON_H__

#include <map>
#include <string>

enum net_buffer_len_e
{
    kRECV_BUFF_LEN = 512 * 1024,
    kSEND_BUFF_LEN = 128 * 1024,
    kRECV_QUEUE_BUFF_LEN = 512 * 1024,
    kSEND_QUEUE_BUFF_LEN = 128 * 1024,
};

enum { kHTTP_TAG_MAXLEN = 256};     // HTTP Request tag max lengh limit

enum http_request_type_e
{
    kHTTP_GET,
    kHTTP_POST,
};

enum enmConnStat
{
    kSocketConnSuccess  = 0,
    kSocketConnFail     = -101,     // can not connect to destination address
    kSocketConnBreak    = -102,     // connecting with dest address broken

    kReconnectCount     = 3,
};

class URLVariables
{
public:
    struct _url_value
    {
        std::string value;

#define _val_convert_to_string(_dst, _fmt, _val)                    \
        do {                                                        \
            char sz[32] = {0}; sprintf(sz, _fmt, _val); _dst = sz;  \
        } while (0);

        _url_value(const std::string &_val) { this->value = _val; }
        _url_value(const char *_val) 
        { 
            if (_val) { this->value = _val; }
            else { this->value = ""; }
        }
        _url_value(int _val) 
        {
            _val_convert_to_string(this->value, "%d", _val);
        }
        _url_value(unsigned int _val) 
        {
            _val_convert_to_string(this->value, "%u", _val);
        }
        _url_value(long int _val) 
        {
            _val_convert_to_string(this->value, "%l", _val);
        }
        _url_value(unsigned long _val) 
        {
            _val_convert_to_string(this->value, "%lu", _val);
        }
        _url_value(long long _val) 
        {
#ifdef _WIN32
            _val_convert_to_string(this->value, "%I64d", _val);
#else
            _val_convert_to_string(this->value, "%ll", _val);
#endif
        }
        _url_value(unsigned long long _val) 
        {
#ifdef _WIN32
            _val_convert_to_string(this->value, "%I64u", _val);
#else
            _val_convert_to_string(this->value, "%llu", _val);
#endif
        }
        _url_value(float _val) 
        {
            _val_convert_to_string(this->value, "%f", _val);
        }
        _url_value(double _val) 
        {
            _val_convert_to_string(this->value, "%lf", _val);
        }

        _url_value& operator = (const std::string &_val)
        {
            this->value = _val;
            return *this;
        }
        _url_value& operator = (const char *_val)
        {
            if (_val) { this->value = _val; }
            else { this->value = ""; }
            return *this;
        }
        _url_value& operator = (int _val)
        {
            _val_convert_to_string(this->value, "%d", _val);
            return *this;
        }
        _url_value& operator = (unsigned int _val)
        {
            _val_convert_to_string(this->value, "%u", _val);
            return *this;
        }
        _url_value& operator = (long int _val)
        {
            _val_convert_to_string(this->value, "%l", _val);
            return *this;
        }
        _url_value& operator = (unsigned long _val)
        {
            _val_convert_to_string(this->value, "%lu", _val);
            return *this;
        }
        _url_value& operator = (long long _val) 
        {
#ifdef _WIN32
            _val_convert_to_string(this->value, "%I64d", _val);
#else
            _val_convert_to_string(this->value, "%ll", _val);
#endif
            return *this;
        }
        _url_value& operator = (unsigned long long _val) 
        {
#ifdef _WIN32
            _val_convert_to_string(this->value, "%I64u", _val);
#else
            _val_convert_to_string(this->value, "%llu", _val);
#endif
            return *this;
        }
        _url_value& operator = (float _val)
        {
            _val_convert_to_string(this->value, "%f", _val);
            return *this;
        }
        _url_value& operator = (double _val)
        {
            _val_convert_to_string(this->value, "%lf", _val);
            return *this;
        }
    };

    typedef std::map<std::string, _url_value>  variable_map_t;
public:
    URLVariables() {}
    ~URLVariables() {}

    std::string decode() const
    {
        std::string rel;
        for (variable_map_t::const_iterator iter = m_variables.begin();
            iter != m_variables.end();)
        {
            rel.append(iter->first.c_str());
            rel.append("=");
            rel.append(iter->second.value.c_str());
            ++iter;
            if (iter != m_variables.end())
            {
                rel.append("&");
            }
        }
        return rel;
    }

    _url_value & operator[](const _url_value & _Keyval)
    {
        variable_map_t::iterator _Where = m_variables.find(_Keyval.value.c_str());
        if (_Where == m_variables.end())
        {
            _Where = m_variables.insert(_Where, 
                std::make_pair(_Keyval.value.c_str(), _url_value("")));
            //_Where = m_variables.find(_Keyval);
        }
        return ((*_Where).second);
    }

private:
    variable_map_t  m_variables;
};

#endif
