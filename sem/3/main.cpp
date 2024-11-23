#include <iostream>
struct Time {
    Time(int hours, int minutes, int seconds)
        : seconds(seconds), minutes(minutes), hours(hours) {
        Normalize();
    }

    void InternalPrint() {
        std::cout << "HH:MM:SS\n"
                  << hours << ":" << minutes << ":" << seconds << std::endl;
    }

    void SetHours(int hours) {
        this->hours = hours;
        Normalize();
    };

    void SetMinutes(int minutes) {
        this->minutes = minutes;
        Normalize();
    };

    void SetSeconds(int seconds) {
        this->seconds = seconds;
        Normalize();
    }

    int GetHours() const {
        return hours;
    }

    int GetMinutes() const {
        return minutes;
    }

    int GetSeconds() const {
        return seconds;
    }

    void Normalize() {
        if (seconds < 0) {
            minutes += (seconds - 59) / 60;
            seconds = (60 + seconds % 60) % 60;
        } else if (seconds >= 60) {
            minutes += seconds / 60;
            seconds %= 60;
        }

        if (minutes < 0) {
            hours += (minutes - 59) / 60;
            minutes = (60 + minutes % 60) % 60;
        } else if (minutes >= 60) {
            hours += minutes / 60;
            minutes %= 60;
        }

        hours = (24 + hours % 24) % 24;
    }

    void operator+=(const Time &other) {
        seconds += other.seconds;
        minutes += other.minutes;
        hours += other.hours;
        Normalize();
    }

    Time operator+(const Time &other) const {
        Time self = *this;
        self += other;
        return self;
    }

    void operator-=(const Time &other) {
        seconds -= other.seconds;
        minutes -= other.minutes;
        hours -= other.hours;
        Normalize();
    }

    Time operator-(const Time &other) const {
        Time self = *this;
        self -= other;
        return self;
    }

    bool operator==(const Time &other) const {
        Time a = *this;
        a.Normalize();
        Time b = other;
        b.Normalize();
        return a.seconds == b.seconds && a.minutes == b.minutes &&
               a.hours == b.hours;
    }

  private:
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
};

int main() {
    Time a(0, 0, 24 * 60 * 60 - 1);
    Time b(0, 0, -1);
    std::cout << (a == b) << std::endl;

    Time c(5, 4, 0);
    Time d(3, 2, 1);
    (c - d).InternalPrint();
}