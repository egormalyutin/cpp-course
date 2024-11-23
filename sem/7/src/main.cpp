#include <list>
#include <memory>
#include <stdexcept>
#include <vector>

#include <iomanip>
#include <iostream>
#include <stdexcept>

struct Time;

struct Watch {
    Watch(Time &time) : time(time), fmt(24) {
    }

    int get_seconds() const;
    int get_minutes() const;
    int get_hours() const;

    void reset();

    void format(int format) {
        if (format != 24 && format != 12) {
            throw std::runtime_error("invalid format");
        }
        fmt = format;
    }

  private:
    Time &time;
    int fmt;

    friend std::ostream &operator<<(std::ostream &output, const Watch &time);
};

std::ostream &operator<<(std::ostream &output, const Watch &time) {
    if (time.fmt == 24) {
        output << std::setfill('0') << std::setw(2) << time.get_hours();
        output << ":" << std::setw(2) << time.get_minutes();
        output << ":" << std::setw(2) << time.get_seconds();
        output << " on my watch";
    } else {
        output << std::setfill('0') << std::setw(2) << time.get_hours() % 12;
        output << ":" << std::setw(2) << time.get_minutes();
        output << ":" << std::setw(2) << time.get_seconds();
        if (time.get_hours() >= 12) {
            output << " pm";
        } else {
            output << " am";
        }
        output << " on my watch";
    }
    return output;
}

struct Watch;

struct Time {
    Time() noexcept : Time(0, 0, 0) {
    }

    Time(int hours, int minutes, int seconds) noexcept
        : hours(hours), minutes(minutes), seconds(seconds) {
        renormalize();
    }

    int get_hours() const noexcept {
        return hours;
    }

    int get_minutes() const noexcept {
        return minutes;
    }

    int get_seconds() const noexcept {
        return seconds;
    }

    void set_hours(int hours) noexcept {
        this->hours = hours;
        renormalize();
    };

    void set_minutes(int minutes) noexcept {
        this->minutes = minutes;
        renormalize();
    };

    void set_seconds(int seconds) noexcept {
        this->seconds = seconds;
        renormalize();
    }

    void operator+=(const Time &other) noexcept {
        seconds += other.seconds;
        minutes += other.minutes;
        hours += other.hours;
        renormalize();
    }

    void operator+=(int seconds) noexcept {
        this->seconds += seconds;
        renormalize();
    }

    Time operator+(const Time &other) const noexcept {
        return Time(seconds + other.seconds, minutes + other.minutes,
                    hours + other.hours);
    }

    Time operator+(int seconds) const noexcept {
        return Time(this->seconds + seconds, minutes, hours);
    }

    void operator-=(const Time &other) noexcept {
        seconds -= other.seconds;
        minutes -= other.minutes;
        hours -= other.hours;
        renormalize();
    }

    void operator-=(int seconds) noexcept {
        this->seconds -= seconds;
        renormalize();
    }

    Time operator-(const Time &other) const noexcept {
        return Time(seconds - other.seconds, minutes - other.minutes,
                    hours - other.hours);
    }

    Time operator-(int seconds) const noexcept {
        return Time(this->seconds - seconds, minutes, hours);
    }

    bool operator==(const Time &other) const noexcept {
        return seconds == other.seconds && minutes == other.minutes &&
               hours == other.hours;
    }

    long to_seconds() const noexcept {
        return hours * 3600 + minutes * 60 + seconds;
    }

    void break_everything() const {
        throw std::runtime_error("AAAAAAAA");
    }

  private:
    int hours = 0;
    int minutes = 0;
    int seconds = 0;

    void renormalize() noexcept {
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

    friend class SimpleWatch;
    friend int Watch::get_seconds() const;
    friend int Watch::get_minutes() const;
    friend int Watch::get_hours() const;
    friend void Watch::reset();
};

static std::ostream &operator<<(std::ostream &output, const Time &time) {
    output << std::setfill('0') << std::setw(2) << time.get_hours();
    output << ":" << std::setw(2) << time.get_minutes();
    output << ":" << std::setw(2) << time.get_seconds();
    return output;
}

int Watch::get_hours() const {
    return time.hours;
}

int Watch::get_minutes() const {
    return time.minutes;
}

int Watch::get_seconds() const {
    return time.seconds;
}

void Watch::reset() {
    time.set_hours(0);
    time.set_minutes(0);
    time.set_seconds(0);
}

struct SimpleWatch {
    SimpleWatch(Time &time) : time(time), fmt(24) {
    }

    int get_seconds() const {
        return time.seconds;
    }

    int get_minutes() const {
        return time.minutes;
    }

    int get_hours() const {
        return time.hours;
    }

    void reset() {
        time.set_hours(0);
        time.set_minutes(0);
        time.set_seconds(0);
    }

    void format(int format) {
        if (format != 24 && format != 12) {
            throw std::runtime_error("invalid format");
        }
        fmt = format;
    }

  private:
    Time &time;
    int fmt;

    friend std::ostream &operator<<(std::ostream &output,
                                    const SimpleWatch &time);
};

std::ostream &operator<<(std::ostream &output, const SimpleWatch &time) {
    if (time.fmt == 24) {
        output << std::setfill('0') << std::setw(2) << time.get_hours();
        output << ":" << std::setw(2) << time.get_minutes();
        output << ":" << std::setw(2) << time.get_seconds();
        output << " on my watch";
    } else {
        output << std::setfill('0') << std::setw(2) << time.get_hours() % 12;
        output << ":" << std::setw(2) << time.get_minutes();
        output << ":" << std::setw(2) << time.get_seconds();
        if (time.get_hours() >= 12) {
            output << " pm";
        } else {
            output << " am";
        }
        output << " on my watch";
    }
    return output;
}

struct CuckooClock : public SimpleWatch {
    CuckooClock(Time &time) : SimpleWatch(time) {
        std::cout << "cuckoo clock created" << std::endl;
    }

    std::string cuckoo() {
        return "cuckoo";
    }

    ~CuckooClock() {
        std::cout << "cuckoo clock destroyed" << std::endl;
    }
};

struct WallClock : public SimpleWatch {
    WallClock(Time &time) : SimpleWatch(time) {
        std::cout << "wall clock created" << std::endl;
    }

    void tick() {
        std::cout << "tick" << std::endl;
    }

    ~WallClock() {
        std::cout << "wall clock destroyed" << std::endl;
    }
};

struct SmartWatch : public SimpleWatch {
    SmartWatch(Time &time) : SimpleWatch(time) {
        std::cout << "smart watch created" << std::endl;
    }

    int smartness() {
        return 10;
    }

    ~SmartWatch() {
        std::cout << "smart watch destroyed" << std::endl;
    }
};

struct PlainWatch : public SimpleWatch {
    PlainWatch(Time &time) : SimpleWatch(time) {
        std::cout << "plain watch created" << std::endl;
    }

    int smartness() {
        return 0;
    }

    ~PlainWatch() {
        std::cout << "plain watch destroyed" << std::endl;
    }
};

int main() {
    Time t(13, 2, 3);
    SimpleWatch w(t);
    w.format(12);
    std::cout << w << std::endl;
    w.reset();
    std::cout << w << std::endl;
    std::cout << t << std::endl;

    t = Time(13, 2, 3);
    Watch w2(t);
    w2.format(12);
    std::cout << w2 << std::endl;
    w2.reset();
    std::cout << w2 << std::endl;
    std::cout << t << std::endl;

    CuckooClock clock(t);
}