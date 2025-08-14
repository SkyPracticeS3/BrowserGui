#pragma once
#include <functional>

template<typename ...FuncArgs>
class Event {
public:
    std::function<void(FuncArgs...)> func;

    void emit(FuncArgs... args) {
        func(args...);
    };

    void connect(std::function<void(FuncArgs...)> function){
        func = std::move(function);
    }
};

template<>
class Event<> {
public:
    std::function<void()> func;

    void emit() {
        func();
    };

    void connect(std::function<void()> function){
        func = std::move(function);
    }
};