# hello_word

bool Protester::commonA()
{
    if (getState() == 0)
        return true;
    
    if (m_tick != 0)
    {
        m_tick--;
        return true;
    }
    m_tick = max(0, 3 -getWorld()->getLevel()/4);
    if (getState() == 1)
    {
        if (getX() == 60 && getY()==60)
        {
            setdead();
            return true;
        }
        setDirection(getWorld()->getpath(getX(),getY()));
        int a=getX(), b=getY();
        movable(a, b, getDirection());
        moveTo(a, b);
        return true;
    }
    if (getState() == 2)
    {
        if (yell())
            return true;
       
    }

    return false;
}
bool Protester::commonB()
{
    if (chase())
            return true;
    return false;
    
}
bool Regularp::differentA()
{
    decrementstep();
    if (getnumstep() <=0)
    {   runsoutstep();
    return true;
    }
    return false;
}
bool Hardcorep::differentA()
{
    decrementstep();
    if (getnumstep() <= 0)
        runsoutstep();
    return false;
}

void Protester::commonC()
{
    if (differentA())
        ;
    else if (!intersection())
        perpendicular--;

    int a = getX(),b=getY();
    if (movable(a, b, getDirection()))
    {
        moveTo(a,b);
    }
    else
        numStep = 0;
    
    return;
}
void Protester::annoy()
{
    if (getState() == 1)
        return;
    if (getState() == 2)
    {
    if (getlife() <= 0)
    {
        setState(1);
        getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
        m_tick = 0;
        if (getlife() <= -95)
        {
            getWorld()->increaseScore(500);
            setState(1);
        }
        else
        {   if (getname() == 'R')
                getWorld()->increaseScore(100);
            else
                getWorld()->increaseScore(250);
        }
        return;
    }
    else
    {   getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
        stare();
        return;
    }
    }
}

//need a funciton in student world to inform frackman
bool Protester::yell()
{
    if (getWorld()->couldyell(getX(), getY(), getDirection()))
    {
    getWorld()->playSound(SOUND_PROTESTER_YELL);
    getWorld()->getattack('p');
    m_tick = 15* max(0, 3 -getWorld()->getLevel()/4);
    return true;
    }
    return  false;
}
//continue step 8
void Protester::runsoutstep()
{
    for(;;)
    {   Direction dir = randdir();
        int a=getX(),b=getY();
            if (movable(a,b,dir))
            {
                setDirection(dir);
                numStep = randInt(8, 60);
                return;
            }
    }
  
}
//continue step 8
bool Protester::intersection()
{
    if (perpendicular > 0)
        return false;
    Direction dir = getWorld()->couldturn(getX(), getY(), getDirection());
    if (dir != none)
    {
        setDirection(dir);
        numStep = randInt(8, 60);
        perpendicular = 200;
        return true;
    }
    return false;
}
//IF TRUE, immediately return
bool Protester::chase()
{   int a=getX(),b=getY();
    Direction dir =getWorld()->couldchase(getX(), getY());
    if (dir != none)
    {
        setDirection(dir);
        supposemove(a,b,dir);
        moveTo(a,b);
        numStep = 0;
        return true;
    }
    return false;
        
}
void Protester::stare()
{
    m_tick = max(50, 100 - getWorld()->getLevel() * 10);
}
Regularp::Regularp(StudentWorld *s)
:Protester(IID_PROTESTER,s,'R'){setlife(5);}

void Regularp::doSomething()
{
    if (commonA())
        return;
    if (commonB())
        return;
    commonC();
    
}

Hardcorep::Hardcorep(StudentWorld *s)
:Protester(IID_HARD_CORE_PROTESTER,s,'H'){setlife(20);}

void Hardcorep::doSomething()
{
    if (commonA())
        return;
    if (follow())
        return;
    if (commonB())
        return;
    commonC();
}
