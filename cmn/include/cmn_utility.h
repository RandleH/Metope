





#ifndef CMN_UTILITY_H
#define CMN_UTILITY_H



#define CMN_SWAP( a, b)   do{ (a)^=(b); (b)^=(a); (a)^=(b); }while(0)

#define CMN_MIN( a, b)    ((a)>(b) ? (b) : (a))
#define CMN_MAX( a, b)    ((a)<(b) ? (b) : (a))


#endif