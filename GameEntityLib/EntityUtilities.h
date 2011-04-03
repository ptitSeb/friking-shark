#pragma once


double  ApproachAngle(double actual,double ideal, double ammount);
void    ApproachAngles(CVector *pvCurrent,CVector &vIdeal, double ammount);
double 	AngleDiff(double dAngle1,double dAngle2);
CVector PredictInterceptionPosition(CVector vInterceptorPos,double dInterceptorVel,CVector vTargetPosition,CVector vTargetVelocity);
