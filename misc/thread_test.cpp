/*************************************************************************
 *
 * Copyright (c) 2010 Kohei Yoshida
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 ************************************************************************/

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include <queue>

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/mutex.hpp>

#include <stdio.h>
#include <string>
#include <sys/time.h>

using namespace std;
using ::boost::mutex;

namespace {

mutex tprintf_mtx;

void tprintf(const string& s)
{
    mutex::scoped_lock(tprintf_mtx);
    cout << s << endl;
    cout.flush();
}

class StackPrinter
{
public:
    explicit StackPrinter(const char* msg) :
        msMsg(msg)
    {
        string s = msg + string(": --begin");
        tprintf(s);
        mfStartTime = getTime();
    }

    ~StackPrinter()
    {
        double fEndTime = getTime();
        ostringstream os;
        os << msMsg << ": --end (durtion: " << (fEndTime-mfStartTime) << " sec)";
        tprintf(os.str());
    }

private:
    double getTime() const
    {
        timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    }

    ::std::string msMsg;
    double mfStartTime;
};

}

class formula_cell
{
public:
    formula_cell() : mp_thread(NULL) {}

    void interpret_thread()
    {
        mp_thread = new ::boost::thread(::boost::bind(&formula_cell::interpret, this));
    }

    void interpret()
    {
        StackPrinter __stack_printer__("formula_cell::interpret");
        sleep(1);
    }

    void interpret_join()
    {
        mp_thread->join();
        delete mp_thread;
        mp_thread = NULL;
    }
private:
    ::boost::thread* mp_thread;
};

struct manage_queue_data
{
    ::boost::mutex mtx;
    ::boost::condition_variable cond;
    queue<formula_cell*> cell_queue;

    bool ready;
    bool added_to_queue;
    bool terminate_requested;

    manage_queue_data() :
        ready(false),
        added_to_queue(false), 
        terminate_requested(false) {}
};

manage_queue_data queue_data;

void manage_queue_main()
{
    StackPrinter __stack_printer__("::manage_queue_main");
    ::boost::mutex::scoped_lock lock(queue_data.mtx);
    while (!queue_data.terminate_requested)
    {
        tprintf("waiting...");
        queue_data.ready = true;
        queue_data.cond.wait(lock);
        if (queue_data.added_to_queue)
        {
            while (!queue_data.cell_queue.empty())
            {
                formula_cell* p = queue_data.cell_queue.front();
                queue_data.cell_queue.pop();
                p->interpret();
            }
            queue_data.added_to_queue = false;
        }
    }

    tprintf("finishing up...");
    while (!queue_data.cell_queue.empty())
    {
        formula_cell* p = queue_data.cell_queue.front();
        queue_data.cell_queue.pop();
        p->interpret();
    }
}

void add_to_queue(formula_cell* p)
{
    tprintf("adding to queue...");
    ::boost::mutex::scoped_lock lock(queue_data.mtx);
    queue_data.cell_queue.push(p);
    queue_data.added_to_queue = true;
    queue_data.cond.notify_all();
}

void manage_queue_terminate()
{
    ::boost::mutex::scoped_lock lock(queue_data.mtx);
    queue_data.terminate_requested = true;
    queue_data.cond.notify_all();
}

int main()
{
    StackPrinter __stack_printer__("::main");
    ::boost::thread thr_queue(manage_queue_main);
    sleep(1);

    ::boost::ptr_vector<formula_cell> cells;
    cells.push_back(new formula_cell);
    cells.push_back(new formula_cell);
    cells.push_back(new formula_cell);
    cells.push_back(new formula_cell);
    cells.push_back(new formula_cell);
    cells.push_back(new formula_cell);

    ::boost::ptr_vector<formula_cell>::iterator itr, itr_beg = cells.begin(), itr_end = cells.end();
    for (itr = itr_beg; itr != itr_end; ++itr)
    {
        formula_cell* p = &(*itr);
        add_to_queue(p);
    }

    manage_queue_terminate();
    thr_queue.join();
    fprintf(stdout, "main:   final queue size = %d\n", queue_data.cell_queue.size());
}
