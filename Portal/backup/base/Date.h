/**
 * 2022.03.16
 * 日期
 *
 */

#ifndef PORTAL_BASE_DATE_H
#define PORTAL_BASE_DATE_H

#include "./copyable.h"
#include "./Types.h"

struct tm;

namespace portal
{

class Date : public portal::copyable
{
    public:
        struct YearMonthDay
        {
            int year;   // [1900..2500]
            int month;  // [1..12]
            int day;    // [1..31]
        };

        static const int kDayPerWeek = 7;
        static const int kJulianDayOf1970_01_01;

        Date()
            : julianDayNumber_(0)
        {  
        }

        Date(int year, int month, int day);

        explicit Date(int julianDayNum)
            : julianDayNumber_(julianDayNum)
        {
        }

        explicit Date(const struct tm&);

        void swap(Date &that)
        {
            std::swap(julianDayNumber_, that.julianDayNumber_);
        }

        bool valid() const
        {
            return julianDayNumber_ > 0;
        }

        string toIsoString() const;

        struct YearMonthDay yearMonthDay() const;

        int year() const
        {
            return yearMonthDay().year;
        }

        int month() const 
        {
            return yearMonthDay().month;
        }

        int day() const 
        {
            return yearMonthDay().day;
        }

        // [0, 1, ..., 6] => [Sunday, Monday, ..., Saturday ]
        int weekDay() const 
        {
            return (julianDayNumber_ + 1) % kDayPerWeek;
        }

        int julianDayNumber() const 
        {
            return julianDayNumber_;
        }

    private:
        int julianDayNumber_;
};


inline bool operator<(Date x, Date y)
{
    return x.julianDayNumber() < y.julianDayNumber();
}

inline bool operator==(Date x, Date y)
{
    return x.julianDayNumber() == y.julianDayNumber();
}


};


#endif

