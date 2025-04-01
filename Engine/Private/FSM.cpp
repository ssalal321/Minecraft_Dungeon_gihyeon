#include "FSM.h"

FSM* FSM::Create()
{
    return new FSM();
}


void FSM::Free()
{
	__super::Free();
}
