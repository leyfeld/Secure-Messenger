#ifndef SERVERERROR_H
#define SERVERERROR_H


enum class ServerError
{
    Success = 0,
    NameInDbError = 101,
    IncorrectLogin = 102,
    IncorrectPassword = 103,
    ServerDisconnected =104,
    ClientDisconnected =105,
    LoginExists =106
};

#endif // SERVERERROR_H
