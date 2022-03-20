/**
 * 2022.03.16
 * 异常类
 *
 */

#ifndef PORTAL_BASE_EXCEPTION_H
#define PORTAL_BASE_EXCEPTION_H

#include <exception>
#include <string>

namespace portal
{

class Exception : public std::exception
{
    public:
        explicit Exception(const char *what);
        explicit Exception(const string &what);
        virtual ~Exception() throw();
        virtual const char *what() const throw();
        const char *stackTrace() const throw();

    private:
        void fillStackTrace();

        string message_;
        string stack_;
};


};


#endif

