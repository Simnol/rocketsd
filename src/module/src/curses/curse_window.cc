#include "curse_window.hh"

#include <thread>

#include <ncurses.h>

#define PAIR_DEFAULT 1
#define PAIR_RED     2
#define PAIR_BLUE    3
#define PAIR_GREEN   4

namespace modules::curses {

std::mutex mutex;

CurseOutputWindow::CurseOutputWindow(std::string name, int y, int x, int h, int w)
{
    window_ = (void*)newwin(h, w, y, x);
    name_ = name;

    getmaxyx((WINDOW*)window_, rows_, cols_);
    rows_ -= 2;
    cols_ -= 2;

    lines_.push_back("");
    refresh();
}

CurseOutputWindow::~CurseOutputWindow() {}

void CurseOutputWindow::write(char c)
{
    std::string& line = lines_.back();

    if (c != '\n') {
        line.push_back(c);
    }

    if (line.size() == cols_ || c == '\n') {
        lines_.push_back("");
        lines_.back().reserve(cols_);

        if (lines_.size() > rows_ + 1) {
            lines_.erase(lines_.begin());
        }

        refresh();
    }
}

void CurseOutputWindow::write(std::string str)
{
    if (lines_.back() == "") {
        lines_.back() = str;
    } else {
        lines_.push_back(str);
        lines_.push_back("");
        lines_.back().reserve(cols_);
    }

    if (lines_.size() > rows_) {
        lines_.erase(lines_.begin());
    }
    refresh();
}

void CurseOutputWindow::refresh()
{
    const std::lock_guard<std::mutex> lock(mutex);
    WINDOW* window = (WINDOW*)window_;

    wattron(window, COLOR_PAIR(PAIR_DEFAULT));
    werase(window);
    box(window, 0, 0);
    wmove(window, 0, 2);
    waddstr(window, name_.c_str());

    int i = 1;
    for (const std::string& line : lines_) {
        wmove(window, i, 1);
        waddstr(window, line.c_str());
        i++;
    }

    wrefresh(window);
    wattroff(window, COLOR_PAIR(PAIR_DEFAULT));
}

CurseInputWindow::CurseInputWindow(int y, int x, int h, int w, std::function<std::pair<std::string, bool>(std::string)> callback)
    : callback_(callback)
{
    window_ = (void*)newwin(h, w, y, x);

    getmaxyx((WINDOW*)window_, rows_, cols_);
    rows_ -= 2;
    cols_ -= 2;

    refresh();

    quit_ = false;
    new std::thread(&CurseInputWindow::run, this);
}

CurseInputWindow::~CurseInputWindow()
{
    quit_ = true;
}

void CurseInputWindow::run()
{
    int c;

    timeout(100);
    while (true) {
        if (quit_) {
            return;
        }

        if ((c = getch()) != EOF) {
            {
                const std::lock_guard<std::mutex> lock_line(line_mutex_);

                switch (c) {
                case '\n':
                    results_.push_back(std::make_pair(line_, callback_(line_)));
                    if (results_.size() * 2 > rows_ - 1) {
                        results_.erase(results_.begin());
                    }

                    line_ = "";
                    cursor_ = results_.size();
                    break;
                case KEY_BACKSPACE:
                    if (line_.size()) {
                        line_.pop_back();
                    }
                    break;
                case KEY_UP:
                    if (cursor_ > 0) {
                        cursor_--;
                        line_ = results_[cursor_].first;
                    }
                    break;
                case KEY_DOWN:
                    if (cursor_ < results_.size()) {
                        cursor_++;
                        line_ = (cursor_ == results_.size()) ? "" : results_[cursor_].first;
                    }
                    break;
                default:
                    line_.push_back(c);
                }
            }
            refresh();
        }

    }
}

void CurseInputWindow::refresh()
{
    const std::lock_guard<std::mutex> lock(mutex);
    const std::lock_guard<std::mutex> lock_line(line_mutex_);

    WINDOW* window = (WINDOW*)window_;

    werase(window);
    box(window, 0, 0);
    wmove(window, 0, 2);
    waddstr(window, "Input");

    int i = 0;
    for (const auto& result : results_) {
        wmove(window, i + rows_ - (results_.size() * 2) - 1, 1);
        waddstr(window, "< ");
        waddstr(window, result.first.c_str());

        wattron(window, COLOR_PAIR(result.second.second ? PAIR_GREEN : PAIR_RED));
        wmove(window, i + rows_ - (results_.size() * 2), 1);
        waddstr(window, "> ");
        waddstr(window, result.second.first.c_str());
        wattroff(window, COLOR_PAIR(result.second.second ? PAIR_GREEN : PAIR_RED));

        i += 2;
    }

    wmove(window, rows_, 1);
    waddstr(window, "> ");
    wattron(window, COLOR_PAIR(PAIR_BLUE));
    waddstr(window, line_.c_str());
    wattroff(window, COLOR_PAIR(PAIR_BLUE));
    wrefresh(window);
}

void init()
{
    initscr();
    use_default_colors();
    cbreak();
    noecho();
    keypad(stdscr, true);
    start_color();
    init_pair(PAIR_DEFAULT, -1, -1);
    init_pair(PAIR_RED, COLOR_RED, -1);
    init_pair(PAIR_BLUE, COLOR_BLUE, -1);
    init_pair(PAIR_GREEN, COLOR_GREEN, -1);
}

void deinit()
{
    endwin();
}

void forceRefresh()
{
    wrefresh(curscr);
}

void maxsize(int& y, int& x)
{
    getmaxyx(stdscr, y, x);
}

} // namespaces