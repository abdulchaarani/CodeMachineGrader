#pragma once

class Instruction {
    virtual void execute() = 0;
};

class Add : public Instruction {
    void execute() override {}
};