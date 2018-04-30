
#include"node.h"


node::node()
{
    failTimer=new QTimer(this);
    isFailed=0;
    connect(failTimer,SIGNAL(timeout()),this,SLOT(myTimeout()));
}



void node::myTimeout()
{
    isFailed=1;
    pTable->hide();
}
