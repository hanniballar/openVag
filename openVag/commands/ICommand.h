#pragma once
class ICommand
{
public:
    virtual void execute();
protected:
    virtual void doAct() = 0;
    virtual void undoAct() = 0;

    bool doFlag = true;
};

