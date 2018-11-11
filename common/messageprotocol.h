#ifndef MESSAGEPROTOCOL_H
#define MESSAGEPROTOCOL_H
enum class LoginAndSmsProtocol
{
    registration = 1,
    login = 2,
    mes = 3,
    reqwestFileInfo = 4,
    sendFile = 5,
    sendChatList = 6,
    answerSendFile = 7,
    requestMessage=8,
    answerMessage=9,
    sendPublicKey = 10,
    deleteClient = 11
//getOneAnswerMessage=10

};
#endif // MESSAGEPROTOCOL_H
