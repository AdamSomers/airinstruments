#ifndef h_Drums
#define h_Drums

#include <iostream>

class Drums
{
public:
    Drums();
    ~Drums();
    void NoteOn(int num, int note, int velocity);
    
    static Drums& instance(void)
    {
        static Drums s_instance;
        return s_instance;
    }

private:
};

#endif
