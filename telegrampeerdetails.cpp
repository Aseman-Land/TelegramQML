#include "telegrampeerdetails.h"

class TelegramPeerDetailsPrivate
{
public:
};

TelegramPeerDetails::TelegramPeerDetails(QObject *parent) :
    TqObject(parent)
{
    p = new TelegramPeerDetailsPrivate;
}

TelegramPeerDetails::~TelegramPeerDetails()
{
    delete p;
}

