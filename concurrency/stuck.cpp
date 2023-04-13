#include <chrono>
#include <iostream>
#include "stuck.h"


std::mutex mutex1 {};

stuck::stuck (int id)
{
    {
        std::scoped_lock<std::mutex> lock (mutex1);
        std::cout << "Constructing stuck object " << id << std::endl;
    }
    this->id = id;
    m_worker_thread = std::thread ([this] { this->loop (); } );
}


stuck::~stuck ()
{
    {
        std::scoped_lock<std::mutex> lock (mutex1);
        std::cout << "Start to destroy stuck object " << id << std::endl;
    }
    {
        m_worker_quit = true;
    }
    m_worker_thread.join();
    {
        std::scoped_lock<std::mutex> lock (mutex1);
        std::cout << "Ready destroying stuck object " << id << std::endl;
    }
}


void stuck::loop ()
{
    {
        std::scoped_lock<std::mutex> lock (mutex1);
        std::cout << "Stuck object " << id << " running" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(5));
    {
        std::scoped_lock<std::mutex> lock (mutex1);
        std::cout << "Stuck object " << id << " getting stuck now" << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(20));
    {
        std::scoped_lock<std::mutex> lock (mutex1);
        std::cout << "Stuck object " << id << " proceeding again" << std::endl;
    }
    m_ready = true;
}


