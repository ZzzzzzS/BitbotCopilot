#include "RTDConnection.h"

RTDConnection::RTDConnection(QWidget *parent)
	: MetaRTDView(RTDViewType::SMALL_WINDOW,parent)
	, ui(new Ui::RTDConnectionClass())
{
	ui->setupUi(this);
}

RTDConnection::~RTDConnection()
{
	delete ui;
}

void RTDConnection::ResetUI()
{
}
