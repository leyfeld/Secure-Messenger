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
    LoginExists =106,
    LoginOffline = 107,
    IncorrectProtocol = 108,
    IncorrectRegistration = 109
};

#endif // SERVERERROR_H
