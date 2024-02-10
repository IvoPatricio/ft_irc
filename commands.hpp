class commands
{
private:
    std::string _nick;
    std::string _username;
    std::string _channel;
    std::string _sendPrivate;
    std::string _receivePrivate;
    std::string _kick;
    std::string _invite;
    std::string _topic;
    std::string _mode;
public:
    commands();
    ~commands();
    void cmdNick(std::string _nick);
    void cmdUsername(std::string _username);
    void cmdChannel(std::string _channel);
    void cmdSendPrivate(std::string _sendPrivate);
    void cmdReceivePrivate(std::string _receivePrivate);
    void cmdKick(std::string _kick);
    void cmdInvite(std::string _invite);
    void cmdTopic(std::string _topic);
    void cmdMode(std::string _mode);

    //setters needed? or inside the CMDs

};