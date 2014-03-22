//****************************************************************************
//Copyright (C) 2005-2006 Beijing BlueDJ Technology Co.,Ltd. All rights reserved.

//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.

//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with this program (in the file LICENSE.GPL); if not, write to the Free Software
//Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

//Please visit http://www.bluedj.com for more infomation about us.
//Contact us at ggwizard@gmail.com or darkdong@gmail.com.
//****************************************************************************/

#include "DJHallController.h"
#include "DJGameController.h"
#include "DJHallInterface.h"
#include "DJGameItem.h"
#include "DJContactItem.h"
#include "DJSocket.h"
#include "DJProgressDialog.h"
#include "DJClientRequest.h"
#include "DJContact.h"
#include "UILogin.h"
#include "UIHallSetup.h"
#include "UIAboutHall.h"
#include "UISelectDownloadServer.h"
#include "HallUtility.h"
#include "DJMainWindow.h"
#include "DJTabWidget.h"
#include "DJGameTab.h"
#include "Utility.h"
#include "GameUtility.h"
#include "LaunchUrl.h"
#include "GameInterface.h"
#include "HallCommand.h"
#include "protocol.h"
#include "DJMessageBox.h"
#include "DJBaseGlobal.h"
#include "DJHallGlobal.h"
#include "DJWidget.h"
#include "DJToolButton.h"
#include "DJGlobal.h"

static	QString		WWW_HOST_DOWNLOAD_URL	= QString(HTTP_WWW_HOST) + "download/download.php";

static const char*		KEY_GAME_SPLITTER_SIZES	= "Hall/GameSplitterSizes";

//version format
//HALL
//significant.major.minor.revise
//BASE
//reserved.major.minor.revise
//GAME
//significant.HallMajor.BaseMajor.game

#if defined( Q_WS_MAC )
	#include "MacWebWidget.h"
	static const quint32	HALL_VERSION	= 0x9030200;
#elif defined( Q_WS_WIN )
	#include "WinWebWidget.h"
	static const quint32	HALL_VERSION	= 0x9030200;
	extern HANDLE	gMutexHandle;
#elif defined( Q_WS_X11 )
	static const quint32	HALL_VERSION	= 0x9030200;
#endif

static const uint		UPDATE_INI_INTERVAL		= 86400;//seconds,1 day
static const int		UPDATE_RES_INTERVAL		= 1000*60*5;//milli seconds,5 min
static const int		BANNED_INTERVAL			= 1000*60*10;//milli seconds,10 min
static const int		GAMELIST_TIMEOUT		= -1;//-1 means never timeout
static const int		SERVERLIST_TIMEOUT		= -1;
static const int		LOGIN_TIMEOUT			= -1;

#ifdef TEST_AUDIO

#include <speex/speex.h>
#include "wave_out.h"

#if defined(Q_WS_WIN)
#include <windows.h>

const int MAX_BLOCKS	= 3;
const int SAMPLES_PER_PERIOD	= 160;//1 period = 1 speex frame size = 20ms
const int PERIODS_PER_BLOCK		= 4;
const int BYTES_PER_SAMPLE		= 2;
const int PERIOD_BUF_SIZE	= SAMPLES_PER_PERIOD * BYTES_PER_SAMPLE;
const int BLOCK_BUF_SIZE	= SAMPLES_PER_PERIOD * BYTES_PER_SAMPLE * PERIODS_PER_BLOCK;

HWAVEIN hWaveIn	= 0;
WAVEHDR waveInHeader[MAX_BLOCKS];
char waveInBuf[MAX_BLOCKS][BLOCK_BUF_SIZE];
char frameBuf[PERIOD_BUF_SIZE];
char blockBuf[BLOCK_BUF_SIZE];
#endif

SpeexBits enc_bits;
void *enc_state;
void CALLBACK waveInProc( HWAVEIN hwi,UINT uMsg, DWORD dwInstance, DWORD dwParam1,DWORD dwParam2 )
{
	QUdpSocket *socket	= reinterpret_cast<QUdpSocket*>(dwInstance);
	switch(uMsg) {
	case WIM_OPEN: {
		djDebug() << "WIM_OPEN";
		enc_state = speex_encoder_init(&speex_nb_mode);
		int quality	= 8;
		speex_encoder_ctl(enc_state,SPEEX_SET_QUALITY,&quality);
		
		speex_bits_init(&enc_bits);
		break;
	}
	case WIM_DATA: {
		djDebug() << "WIM_DATA";
		PWAVEHDR	pWaveInHeader	= (PWAVEHDR)dwParam1;
		//static QFile file("test.wav");
		//file.open( QIODevice::Truncate | QIODevice::WriteOnly );
		//file.write(pWaveInHeader->lpData,pWaveInHeader->dwBytesRecorded);
		//TRACE("dwBytesRecorded =%d\n",pWaveInHeader->dwBytesRecorded);
		
		djDebug() << pWaveInHeader << "dwBytesRecorded=" << pWaveInHeader->dwBytesRecorded;
		//djDebug() << "time =" << QTime::currentTime().toString("hh::mm::ss::zzz");
		//WIN_Play_Samples(pWaveInHeader->lpData, pWaveInHeader->dwBytesRecorded);
		//socket->writeDatagram( pWaveInHeader->lpData,pWaveInHeader->dwBytesRecorded,QHostAddress("10.0.0.50"), 55555);
		//waveInAddBuffer(hwi, pWaveInHeader, sizeof (WAVEHDR)) ;
		speex_bits_reset(&enc_bits); 
		
		char* data	= pWaveInHeader->lpData;
		for ( int i = 0; i < PERIODS_PER_BLOCK; i++ ) {
			
			speex_encode_int(enc_state, (short*)data, &enc_bits); 
			
			//speex_bits_read_from(&dec_bits, oneFrameBuf, nbBytes); 
			//speex_decode_int(dec_state, &dec_bits, oneFrameWaveOutBuf);
			
			//WIN_Play_Samples(oneFrameWaveOutBuf, ONE_FRAME_BUF_SIZE);
			data	+= PERIOD_BUF_SIZE;
		}
		
		int wirteBytes = speex_bits_write(&enc_bits, blockBuf, BLOCK_BUF_SIZE);
		//djDebug() << "wirteBytes" << wirteBytes;
		socket->writeDatagram( blockBuf,wirteBytes,QHostAddress("10.0.0.50"), 55555);
		
		waveInAddBuffer(hwi, pWaveInHeader, sizeof (WAVEHDR)) ;
#if 0
		SpeexBits enc_bits;
		void *enc_state;
		
		enc_state = speex_encoder_init(&speex_nb_mode);
		int quality	= 8;
		speex_encoder_ctl(enc_state,SPEEX_SET_QUALITY,&quality);
		
		speex_bits_init(&enc_bits);
		
		/*
		SpeexBits dec_bits;
		void *dec_state;

		dec_state = speex_decoder_init(&speex_nb_mode); 
		
		int enh	= 1;
		speex_decoder_ctl(dec_state, SPEEX_SET_ENH, &enh);
		//quality	= 1;
		//speex_decoder_ctl(dec_state, SPEEX_SET_QUALITY, &quality);
		
		speex_bits_init(&dec_bits);
		*/
		speex_bits_reset(&enc_bits); 
		
		char* data	= pWaveInHeader->lpData;
		for ( int i = 0; i < PERIODS_PER_BLOCK; i++ ) {
			
			speex_encode_int(enc_state, (short*)data, &enc_bits); 
			
			//speex_bits_read_from(&dec_bits, oneFrameBuf, nbBytes); 
			//speex_decode_int(dec_state, &dec_bits, oneFrameWaveOutBuf);
			
			//WIN_Play_Samples(oneFrameWaveOutBuf, ONE_FRAME_BUF_SIZE);
			data	+= PERIOD_BUF_SIZE;
		}
		
		int wirteBytes = speex_bits_write(&enc_bits, blockBuf, BLOCK_BUF_SIZE);
		//djDebug() << "wirteBytes" << wirteBytes;
		socket->writeDatagram( blockBuf,wirteBytes,QHostAddress("10.0.0.50"), 55555);
		
		waveInAddBuffer(hwi, pWaveInHeader, sizeof (WAVEHDR)) ;
		
		speex_bits_destroy(&enc_bits); 
		speex_encoder_destroy(enc_state); 
		
		//speex_bits_destroy(&dec_bits); 
		//speex_decoder_destroy(dec_state); 
		//djDebug() << "WIM_DATA end";
#endif		
		break;
	}
	case WIM_CLOSE:
		djDebug() << "WIM_CLOSE";
		//speex_bits_destroy(&enc_bits); 
		//speex_encoder_destroy(enc_state); 
		break;
	default:
		djDebug() << "unknown msg";
		break;
	}
	//djDebug() << hex << "dwInstance = " << dwInstance << "dwParam1 = " << dwParam1;
}
#endif

void aclConnectedProc( const QByteArray& buf, const QVariant& parameters )
{
	DJHallController* hallController = static_cast<DJHallController *>(parameters.value<void*>());
	const DJGameProtocol *protocol	= reinterpret_cast<const DJGameProtocol *>(buf.data());
	quint8 status	= protocol->chBuf[0];
	djDebug() << "aclConnectedProc"<<hex<<status;
	if ( DJGAME_PROTOCOL_STATUS_FORBID == status ) {
		hallController->setBanned();
		DJMessageBox::information( 30, 0,
				DJHallController::tr("get game list fail"),
				DJHallController::tr("your IP is banned,please wait for 10 minutes"),
				QMessageBox::Ok);
	}
}
void aclRequestGameListProc( const QByteArray& buf, const QVariant& parameters )
{
	//parameters: socket
	djDebug() << "aclRequestGameListProc";
	DJSocket* socket = static_cast<DJSocket *>(parameters.value<void*>());
	socket->close();
	socket->stopProgress();
}
void aclRequestServerListProc( const QByteArray& buf, const QVariant& parameters )
{
	//parameters: socket
	djDebug() << "aclRequestServerListProc";
	DJSocket* socket = static_cast<DJSocket *>(parameters.value<void*>());
	socket->close();
	socket->stopProgress();
}
void aclLoginProc( const QByteArray& buf, const QVariant& parameters )
{
	//parameters: socket,gameitem
	QList<QVariant> list	= parameters.toList();
	DJSocket* socket = static_cast<DJSocket *>(list.at(0).value<void*>());
	DJGameGameItem* gameItem = static_cast<DJGameGameItem *>(list.at(1).value<void*>());
	DJHallController *hallController	= static_cast<DJHallController *>(list.at(2).value<void*>());
	djDebug()<<(void*)socket<<(void*)gameItem<<"hallController"<<(void*)hallController;
	socket->stopProgress();
	SetSettingLastServer( socket->ip(), socket->port() );
	
	const DJGameProtocol *protocol	= reinterpret_cast<const DJGameProtocol *>(buf.data());
	quint8 status	= protocol->chBuf[0];
	djDebug() << "aclLoginProc"<<hex<<status;
	
	if ( DJGAME_SUCCESS(status) ) {
		djDebug() << "login ok";
		gameItem->gameControllerCommand( GAME_COMMAND_LOGIN_OK, QVariant() );
	}else {
		djDebug() << "login fail";
		hallController->clearLogin();
		socket->removeGame( gameItem->id() );
		if ( DJGAME_PROTOCOL_STATUS_FORBID == status ) {
			hallController->setBanned();
			DJMessageBox::information( 30, 0, DJHallController::tr("Login Fail"),DJHallController::tr("your Account is banned,please wait for 10 minutes"),QMessageBox::Ok);
		}else
			DJMessageBox::information( 30, 0, DJHallController::tr("Login Fail"),DJHallController::tr("invalid username or password"),QMessageBox::Ok);
	}
}

QVariant hiHallCommandProc( int command, const QVariant& parameters )
{
	QList<QVariant> list	= parameters.toList();
	djDebug() << "hiHallCommandProc command = " << command << "list size =" << list.size() << parameters;
	QVariant	result;
	
	switch( command ) {
	case HALL_COMMAND_SEND_REQUEST: {
		DJSocket *socket	= static_cast<DJSocket *>(list.at(0).value<void*>());
		DJClientRequest *request	= static_cast<DJClientRequest *>(list.at(1).value<void*>());
		bool discardable	= list.at(2).toBool();
		bool res	= socket->sendRequest( *request, discardable );
		result	= QVariant( res );
		break;
	}
	case HALL_COMMAND_GAME_QUITTED: {
		DJSocket *socket	= static_cast<DJSocket *>(list.at(0).value<void*>());
		quint16	gameId	= list.at(1).toUInt();
		socket->removeGame( gameId );
		break;
	}
	case HALL_COMMAND_REMOVE_ACL: {
		DJSocket *socket	= static_cast<DJSocket *>(list.at(0).value<void*>());
		QList<void*> *acls	= static_cast<QList<void*>*>(list.at(1).value<void*>());
		socket->removeAcl( *acls );
		break;
	}
	case HALL_COMMAND_USER_ONOFF_LINE: {
		djDebug()<<"HALL_COMMAND_USER_ONOFF_LINE";
		DJContact *contact	= static_cast<DJContact *>(list.at(0).value<void*>());
		quint16 gameId	= list.at(1).toUInt();
		quint32 userId	= list.at(2).toUInt();
		bool	isOnline	= list.at(3).toBool();
		if ( isOnline )
			contact->userOnline( gameId, userId );
		else
			contact->userOffline( gameId, userId );
		break;
	}
	case HALL_COMMAND_USER_CONTACT_TYPE: {
		DJContact *contact	= static_cast<DJContact *>(list.at(0).value<void*>());
		quint32 userId	= list.at(1).toUInt();
		int res	= contact->userType( userId );
		result	= QVariant( res );
		break;
	}
	case HALL_COMMAND_USER_ADD_CONTACT: {
		DJContact *contact	= static_cast<DJContact *>(list.at(0).value<void*>());
		quint16 gameId	= list.at(1).toUInt();
		quint32 userId	= list.at(2).toUInt();
		int contactUserType	= list.at(3).toInt();
		contact->addToContact( gameId, userId, contactUserType );
		break;
	}
	case HALL_COMMAND_USER_REMOVE_CONTACT: {
		DJContact *contact	= static_cast<DJContact *>(list.at(0).value<void*>());
		quint32 userId	= list.at(1).toUInt();
		int contactUserType	= list.at(2).toInt();
		contact->removeFromContact( userId, contactUserType );
		break;
	}
	case HALL_COMMAND_USER_LITERAL_CHAT: {
		DJContact *contact	= static_cast<DJContact *>(list.at(0).value<void*>());
		quint32 userId	= list.at(1).toUInt();
		QString userName	= list.at(2).toString();
		quint16	gameId	= list.at(3).toUInt();
		contact->chatWithUser( gameId, userId, userName );
		break;
	}
	case HALL_COMMAND_CHANGE_SERVER_USERS: {
		djDebug() << "HALL_COMMAND_CHANGE_SERVER_USERS";
		DJHallController *hallController	= static_cast<DJHallController *>(list.at(0).value<void*>());
		quint16 gameId	= list.at(1).toUInt();
		quint16	serverId	= list.at(2).toUInt();
		int	numberOfServerUsers	= list.at(3).toInt();
		djDebug() << hallController << gameId << serverId << numberOfServerUsers;
		hallController->changeServerUsers( gameId, serverId, numberOfServerUsers );
		break;
	}
	case HALL_COMMAND_RECONNECT: {
		DJHallController *hallController	= static_cast<DJHallController *>(list.at(0).value<void*>());
		DJGameGameItem	*gameItem	= static_cast<DJGameGameItem *>(list.at(1).value<void*>());
		DJSocket *socket	= static_cast<DJSocket *>(list.at(2).value<void*>());
		hallController->requestRelogin( gameItem, socket );
		break;
	}
	case HALL_COMMAND_MAIN_WINDOW: {
		DJHallController *hallController	= static_cast<DJHallController *>(list.at(0).value<void*>());
		result	= QVariant::fromValue(static_cast<void*>(hallController->mainWindow()));
		break;
	}
	case HALL_COMMAND_TREE_CONTAINER: {
		DJHallController *hallController	= static_cast<DJHallController *>(list.at(0).value<void*>());
		result	= QVariant::fromValue(static_cast<void*>(hallController->treeContainer()));
		break;
	}
	case HALL_COMMAND_TAB_CONTAINER: {
		DJHallController *hallController	= static_cast<DJHallController *>(list.at(0).value<void*>());
		result	= QVariant::fromValue(static_cast<void*>(hallController->tabContainer()));
		break;
	}
	case HALL_COMMAND_CLOSE_MUTEX_HANDLE: {
#ifdef Q_WS_WIN			
		if ( gMutexHandle ) {
			CloseHandle( gMutexHandle );
			gMutexHandle	= 0;
		}
#endif		
		break;
	}
	case HALL_COMMAND_SELF_LOGIN: {
		QMainWindow *mainWindow	= static_cast<QMainWindow *>(list.at(0).value<void*>());
		QString userName	= list.at(1).toString();
		QString nickName	= list.at(2).toString();
		QString gameName	= qApp->translate("OEM",APPLICATION_NAME);
		QString title	= gameName+ "-" + 
							DJHallController::tr("welcome %1(%2) to the world of %3")
							.arg(userName)
							.arg(nickName)
							.arg(gameName);
		mainWindow->setWindowTitle( title );
		break;
	}
	default:
		djDebug() << "+++++++++++++++++++++++++unknown hall command++++++++++++++++++++++++++";
		break;
	}
	return result;
}

quint32 DJHallController::version()
{
	return HALL_VERSION;
}
void DJHallController::clearLogin()
{
	m_loginName.clear();
	m_loginPassword.clear();
}
void DJHallController::setBanned()
{
	djDebug()<<"DJHallController::setBanned"<<m_isBanned;
	if ( !m_isBanned ) {
		m_isBanned	= true;
		QTimer::singleShot(BANNED_INTERVAL,this,SLOT(clearBanned()));
	}
}
void DJHallController::clearBanned()
{
	djDebug()<<"DJHallController::clearBanned";
	m_isBanned	= false;
}
DJHallController::DJHallController( QMainWindow* mainWindow, QObject *parent )
	:QObject(parent),m_mainWindow(mainWindow)
{   
	
	connect( mainWindow, SIGNAL(quitHall()), SLOT(handleQuitHall()) );
	
	m_isBanned	= false;
	m_baseVersion	= 0;
	m_whbVersion	= 0;
	
    m_mainWindow->setWindowTitle( qApp->translate("OEM",APPLICATION_NAME) );
    QStatusBar*	statusBar	= new QStatusBar;
    statusBar->setMinimumHeight( 24 );
    m_mainWindow->setStatusBar( statusBar );
    
	QSplitter *mainSplitter	= new QSplitter( Qt::Vertical, m_mainWindow );
	m_mainWindow->setCentralWidget( mainSplitter );
	
	createHallBar( mainSplitter );
	
	m_gameSplitter	= new QSplitter( Qt::Horizontal, mainSplitter );
	
	m_treeContainer	= new QStackedWidget(m_gameSplitter);
	
	m_gameTree	= new QTreeWidget;
	PaintBackground( m_gameTree, QPoint(0,0), QPoint(100,100) );
    m_gameTree->setHeaderLabels( QStringList( tr("Game List") ) );
    m_gameTree->header()->setMovable(false);
    
	DJGameItem	*item;
    item	= new DJGameClassItem(DJGAME_GAMECLASS_CARDS,m_gameTree,DJGameItemGameClassType);
    item->setIcon( 0, QIcon( ":/HallRes/image/gameclass/poker.png") );
    item->setText( 0, tr("General Card Games") );
    item	= new DJGameClassItem(DJGAME_GAMECLASS_MAHJONG,m_gameTree,DJGameItemGameClassType);
    item->setIcon( 0, QIcon( ":/HallRes/image/gameclass/domino.png") );
	item->setText( 0, tr("General Domino Games") );
	item	= new DJGameClassItem(DJGAME_GAMECLASS_CHESS,m_gameTree,DJGameItemGameClassType);
	item->setIcon( 0, QIcon( ":/HallRes/image/gameclass/chess.png") );
	item->setText( 0, tr("General Chess Games") );
	item	= new DJGameClassItem(DJGAME_GAMECLASS_COMBAT,m_gameTree,DJGameItemGameClassType);
	item->setIcon( 0, QIcon( ":/HallRes/image/gameclass/duel.png") );
	item->setText( 0, tr("General Combat Games") );
	item	= new DJGameClassItem(DJGAME_GAMECLASS_SINGLE,m_gameTree,DJGameItemGameClassType);
	item->setIcon( 0, QIcon( ":/HallRes/image/gameclass/single.png") );
	item->setText( 0, tr("General Single Games") );
	item	= new DJGameClassItem(DJGAME_GAMECLASS_OTHER,m_gameTree,DJGameItemGameClassType);
	item->setIcon( 0, QIcon( ":/HallRes/image/gameclass/others.png") );
	item->setText( 0, tr("Other Games") );
	
	m_treeContainer->addWidget(m_gameTree);
	
	m_tabContainer	= new DJTabWidget( m_gameSplitter );
	
	connect(m_tabContainer, SIGNAL(rightClicked(int)),SLOT(gameContext(int)));
    m_tabContainer->setTabPosition( QTabWidget::South );
    DJGameTab*	hallTab	= new DJGameTab( 0, Qt::Vertical );
/*
#ifdef Q_WS_WIN	
    //QAxWidget*	homeWidget	= new QAxWidget( hallTab );
    //homeWidget->setControl(QString::fromUtf8("{8856F961-340A-11D0-A96B-00C04FD705A2}"));
	//homeWidget->dynamicCall("Navigate(const QString&)", HTTP_WWW_HOST );
	
	new WinWebWidget( HTTP_WWW_HOST, hallTab );
#endif
*/
#if defined( Q_WS_MAC )
	new MacWebWidget( HTTP_WWW_HOST, m_mainWindow, hallTab );
#elif defined( Q_WS_WIN )
	#if defined( QT_NO_DEBUG ) && !defined( DJ_DEBUG_OUTPUT )
		new WinWebWidget( HTTP_WWW_HOST, hallTab );
	#endif
#elif defined( Q_WS_X11 )
		
#endif
    m_tabContainer->addTab( hallTab,tr("Hall") );
    connect(m_tabContainer, SIGNAL(currentChanged(int)), SLOT(tabChanged(int)));
	
#ifdef Q_WS_MAC	
	QMenu *helpMenu	= m_mainWindow->menuBar()->addMenu( "help" );
	helpMenu->addAction( "about", this, SLOT(about()) );
	helpMenu->addAction(  "preferences", this, SLOT(setup()) );
#endif
	
	m_hallSocket	= new DJSocket( DJSocket::Hall, this );
	
	m_gameTree->setContextMenuPolicy( Qt::CustomContextMenu );
	m_gameContextMenu	= new QMenu( m_gameTree );
	
	m_actQuitHall		= new QAction( tr("Quit hall"), this );
	connect(m_actQuitHall, SIGNAL(triggered()),SLOT(handleQuitHall()));
	
	m_actRefreshGame	= new QAction( tr("refresh game list"), m_gameTree );
	connect(m_actRefreshGame, SIGNAL(triggered()),SLOT(refreshGameList()));
	
	m_actRefreshServer	= new QAction( tr("refresh server list"), m_gameTree );
	connect(m_actRefreshServer, SIGNAL(triggered()),SLOT(refreshServerList()));
	
	m_actQueryGameVersion	= new QAction( tr("query game version"), m_gameTree );
	connect(m_actQueryGameVersion, SIGNAL(triggered()),SLOT(queryGameVersion()));
	
	m_actDownloadIni	= new QAction( tr("check update"), m_gameTree );
	connect(m_actDownloadIni, SIGNAL(triggered()),SLOT(downloadIni()));
	
	connect(m_gameTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem *,int)), SLOT( itemDoubleClicked(QTreeWidgetItem *,int)));
	connect(m_gameTree, SIGNAL(customContextMenuRequested ( const QPoint & )), SLOT( gameTreeContext ( const QPoint & ) ) );
	
	m_downloadManger	= new DJDownloadManager( m_mainWindow, this );
	connect(m_downloadManger,SIGNAL(fileFinished(QFile*,const QString&,int,quint16,const QString&,int)),SLOT(downloadFinished(QFile*,const QString&,int,quint16,const QString&,int)));
	
	reloadSettings();
	
	QSettings settings;
	if ( settings.contains( KEY_GAME_SPLITTER_SIZES) ) {
    	m_gameSplitter->restoreState(settings.value( KEY_GAME_SPLITTER_SIZES ).toByteArray());
    }else {
    	QList<int> sizes;
    	sizes << 100 << 400;
    	m_gameSplitter->setSizes( sizes );
    }
    m_gameSplitter->setOpaqueResize( true );
	
	QTimer::singleShot(0,this,SLOT(launchAppFinished()));
	
	GetSettingResource( m_resources );
	
	m_timer	= new QTimer(this);
	connect(m_timer,SIGNAL(timeout()),SLOT(downloadResource()));
	m_timer->start(UPDATE_RES_INTERVAL);
	
	QDir	tempDir;
	tempDir.setPath( QDir::temp().filePath(TEMP_DIR) );
	tempDir.mkpath(".");
	
	m_httpFile	= new QTemporaryFile( tempDir.filePath("http"), this );
	m_http	= new QHttp( this );
}
DJHallController::~DJHallController()
{
	djDebug()<<"DJHallController destructor";
	delete m_hallSocket;
	m_hallSocket	= 0;

	qDeleteAll( m_gameSockets );
	m_gameSockets.clear();
	
	QSettings settings;
	settings.setValue( KEY_GAME_SPLITTER_SIZES, m_gameSplitter->saveState());
}
void DJHallController::createHallBar( QWidget* parent )
{
	int barHeight	= 72;
	int space	= 32;
	QSize	iconSize(48,48);
	
	DJWidget	*bar	= new DJWidget( parent );	
	bar->setFixedHeight( barHeight );
	QHBoxLayout* layout	= new QHBoxLayout( bar );
	layout->setMargin( 0 );
	layout->setSpacing( space );
	layout->addSpacing( space );
	
	QGradientStops stops;
	stops << QGradientStop( 0, QColor(172,206,247) )
			<< QGradientStop( 0.5, Qt::blue )
			<< QGradientStop( 1, QColor(172,206,247) );
	bar->setStops( stops );
	
	DJToolButton *logo	= new DJToolButton;
	QPixmap	pix	= QPixmap( ":/HallRes/image/bar/logo.png" );
	logo->setIcon( pix );
	logo->setIconSize( pix.size() );
	connect(logo, SIGNAL(clicked()), SLOT(home()) );
	layout->addWidget( logo );
	
	layout->addStretch();
	
	DJToolButton *setup	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	setup->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
#endif
	setup->setText(tr("hall setup"));
	setup->setToolTip(tr("setup hall"));
	setup->setIcon( QIcon(":/HallRes/image/bar/setup.png") );
	setup->setIconSize( iconSize );
	connect( setup, SIGNAL(clicked()), SLOT(setup()) );
	
	DJToolButton *home	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	home->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
#endif
	home->setText(tr("%1 home").arg( qApp->translate("OEM",ORGANIZATION_NAME) ));
	home->setToolTip(tr("%1 home").arg( qApp->translate("OEM",ORGANIZATION_NAME) ));
	home->setIcon( QIcon(":/HallRes/image/bar/home.png") );
	home->setIconSize( iconSize );
	connect(home, SIGNAL(clicked()),SLOT(home()));
	
	DJToolButton *about	= new DJToolButton;
#ifdef DJ_TOOL_BUTTON_HAS_TEXT
	about->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
#endif
	about->setText(tr("about %1").arg( qApp->translate("OEM",APPLICATION_NAME) ));
	about->setToolTip(tr("about %1").arg( qApp->translate("OEM",APPLICATION_NAME) ));
	about->setIcon( QIcon(":/HallRes/image/bar/info.png") );
	about->setIconSize( iconSize );
	connect(about, SIGNAL(clicked()),SLOT(about()));
	
	layout->addWidget( home );
	layout->addWidget( setup );
	layout->addWidget( about );

	layout->addSpacing( space );
}
void DJHallController::handleQuitHall()
{
	djDebug()<<"DJHallController::handleQuitHall";
	/*
	if ( 0 == QMessageBox::information(0,tr("Quit hall"),tr("Quit hall will quit all games and terminate this program"),tr("Quit") ,tr("Cancel")) ) {
		QApplication::quit();
	}
	*/
	
	delete m_hallSocket;
	m_hallSocket	= 0;

	qDeleteAll( m_gameSockets );
	m_gameSockets.clear();
	
	QApplication::quit();
}
void DJHallController::launchAppFinished()
{		
	djDebug() << "launchAppFinished";
	
	UILogin loginDlg;
    if ( QDialog::Accepted == loginDlg.exec() ) {
		m_loginName	= loginDlg.loginUserName();
		m_loginPassword	= loginDlg.loginPassword();
	}
	
	//check if there is unfinished update for hall
	QDir hallDir("update/hall");
	QStringList hallFiles	= hallDir.entryList(QDir::Files);
	if ( !hallFiles.isEmpty() ) {
		if ( 0 == QMessageBox::information(0,tr("Hall update is unfinish"),tr("go on to finish"),tr("Go on") ,tr("Later")) ) {
			RelaunchHallForUpdate();
			return;
		}
	}
	//check if there is unfinished update for base
	QDir baseDir("update/base");
	QStringList baseFiles	= baseDir.entryList(QDir::Files);
	for( int i = 0; i < baseFiles.size(); i++ ) {
		QString fileName	= baseFiles.at(i);
		djDebug() << fileName;
		QString sourceFilePath	= baseDir.filePath( fileName );
		QString targetFilePath	= QString("./") + fileName;
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			;
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
	}
	
	DeleteAllOldModules();
	ReviseModuleName();
	
	loadBaseLibrary();
	
	//check if download ini file for update
	if ( iniExpired() ) {
		downloadIni();
	}
	
	getGameList();
}
void DJHallController::loadBaseLibrary()
{	
	djDebug() << "DJHallController::loadBaseLibrary";
	QDir currentDir("./");
	QStringList files	= currentDir.entryList(QDir::Files);
	foreach( QString file, files ) {
		QLibrary lib( file );
		//if ( QLibrary::isLibrary( file ) ) {
			djDebug() << "isLibrary" << file;
			GIGetBaseVersionProc	giGetBaseVersionProc	= (GIGetBaseVersionProc)(lib.resolve("GetBaseVersion"));
			if ( giGetBaseVersionProc ) {
				lib.load();
				m_baseVersion	= giGetBaseVersionProc();
				m_whbVersion	= (HALL_VERSION & 0xFFFF0000) | (SecondVersion(m_baseVersion) << 8);
				quint32 baseIniVersion	= GetSettingBaseVersion();
				djDebug()<<hex<<"m_baseVersion="<<m_baseVersion<<"m_whbVersion"<<m_whbVersion
						<<"baseIniVersion="<<baseIniVersion;
				int baseMatch	= CheckVersionMatch( baseIniVersion, m_baseVersion );
				if ( baseMatch > 0 ) {
					//can update base
					if ( 0 == QMessageBox::information(0,tr("Update base"),tr("The base library with higher version is available, do you wanna download now?"),tr("Download Now") ,tr("Later")) ) {
						downloadExecutable( DJDownloadManager::Base, 0, UPDATE_INI_SECTION_BASE, tr("Base") );
						break;
					}
				}
			}
		//}
	}
	
	QTranslator* baseTranslator	= new QTranslator(this);
    QString localeName	= FindSupportedLocaleName();
	 baseTranslator->load( QString(":/BaseRes/lang/Base_") + localeName );
    QCoreApplication::installTranslator(baseTranslator);
    	
    extractFaceIcon();
}
void DJHallController::extractFaceIcon()
{
	QDir s( ":/BaseRes/image/face");
	QDir d("images");
	d.mkpath(".");
	
	QStringList files	= s.entryList(QDir::Files);
	for( int i = 0; i < files.size(); i++ ) {
		QString iconName	= files.at(i);
    	QString	iconFilePath	= d.filePath( iconName );
    	if( !QFile::exists( iconFilePath ) ) {
    		QImage icon( s.filePath(iconName) );
    		icon.save( iconFilePath, "png" );
    	}
    }
}
bool DJHallController::iniExpired()
{
	uint lastUpdateDate	= GetSettingLastUpdate();
	if ( 0 == lastUpdateDate )
		return true;
	uint currentDate	= QDateTime::currentDateTime().toTime_t();
	djDebug() << "check date" << lastUpdateDate << currentDate;
	if ( currentDate - lastUpdateDate > UPDATE_INI_INTERVAL ) {
		return true;
	}else
		return false;
}
bool DJHallController::saveChat() const
{
	return m_saveChat;
}
bool DJHallController::popupChat() const
{
	return m_popupChat;
}
void DJHallController::reloadSettings()
{
	djDebug() << "DJHallController::reloadSettings";
	m_language		= GetSettingLanguageCode();
	m_saveChat	= GetSettingSaveChat();
	m_popupChat	= GetSettingPopupChat();
	
	foreach( DJSocket *socket, m_gameSockets ) {
		socket->allGamesReloadSettings();
	}
}
void DJHallController::contactUserTypeChanged( quint32 userId, int contactUserType )
{
	foreach( DJSocket *socket, m_gameSockets ) {
		socket->allGamesContactUserTypeChanged( userId, contactUserType );
	}
}
void DJHallController::setup()
{
	UIHallSetup setupDlg;
	if ( QDialog::Accepted == setupDlg.exec() ) {
		reloadSettings();
	}
}
void DJHallController::home()
{
	LaunchURL( HTTP_WWW_HOST );
	return;
#ifdef TEST_AUDIO	
	waveInStop (hWaveIn);
	waveInClose( hWaveIn );
	
	speex_bits_destroy(&enc_bits); 
	speex_encoder_destroy(enc_state);
#endif	
}
void DJHallController::about()
{
	djDebug() << "about";
	QString hallVersion	= QHostAddress(HALL_VERSION).toString().section('.',1,-1);
	QString baseVersion	= QHostAddress(m_baseVersion).toString().section('.',1,-1);
	UIAboutHall aboutHall( hallVersion, baseVersion );
	aboutHall.exec();
	return;
#ifdef TEST_AUDIO

	WIN_Audio_open();
	
	WAVEFORMATEX	waveInFormat;
	
	waveInFormat.wFormatTag=WAVE_FORMAT_PCM;
	waveInFormat.nChannels=1;
	waveInFormat.nSamplesPerSec=8000;//采样频率
	waveInFormat.nAvgBytesPerSec=8000 *2;
	waveInFormat.nBlockAlign=2;
	waveInFormat.wBitsPerSample=16;
	waveInFormat.cbSize=0;

	QUdpSocket *audioSocket	= new QUdpSocket(this);
	
	if (waveInOpen(&hWaveIn,WAVE_MAPPER,&waveInFormat,(DWORD)waveInProc,(DWORD)audioSocket,CALLBACK_FUNCTION))
	{   //打开录音设备函数 
		//MessageBox("open wave in failed");
		return;
	}
	
	for ( int i = 0; i < MAX_BLOCKS; i++ ) {
		waveInHeader[i].lpData	= waveInBuf[i];
		waveInHeader[i].dwBufferLength	= BLOCK_BUF_SIZE;
		waveInHeader[i].dwBytesRecorded	= 0;
		waveInHeader[i].dwUser	= 0;
		waveInHeader[i].dwFlags	= 0;
		waveInHeader[i].dwLoops	= 0;
		waveInHeader[i].lpNext	= 0;
		waveInHeader[i].reserved	= 0;
		
		if ( MMSYSERR_NOERROR != waveInPrepareHeader( hWaveIn, &waveInHeader[i], sizeof(WAVEHDR) ) ) {
    		waveInClose( hWaveIn );
    		return;
    	}
    
    	if ( MMSYSERR_NOERROR != waveInAddBuffer( hWaveIn, &waveInHeader[i], sizeof (WAVEHDR)) ) {
    		waveInClose( hWaveIn );
    		return;
    	}
	}
	
	waveInStart (hWaveIn) ;
#endif
}
void DJHallController::tabChanged(int index)
{
	djDebug() << "DJHallController::tabChanged" << index;
	DJGameTab* tab	= static_cast<DJGameTab*>(m_tabContainer->widget(index));
	DJGameGameItem*	gameItem	= tab->gameItem();
	if ( gameItem ) {
		gameItem->gameControllerCommand( GAME_COMMAND_GAMETAB_SELECTED, QVariant() );
	}else {
		m_treeContainer->setCurrentIndex(index);
	}
}
void DJHallController::itemDoubleClicked( QTreeWidgetItem * item, int column )
{
	djDebug() << "itemDoubleClicked";
	switch ( item->type() ) {
	case DJGameItemGameType: {
		djDebug() << "DJTreeItemGameType";
		DJGameGameItem	*gameItem	= static_cast<DJGameGameItem *>(item);
		if ( 0 == gameItem->childCount() ) {
			getServerList( gameItem );
		}
		break;
	}
	case DJGameItemServerType: {
		djDebug() << "DJTreeItemServerType";
		DJGameServerItem	*serverItem	= static_cast<DJGameServerItem *>(item);
		DJGameGameItem	*gameItem	= static_cast<DJGameGameItem *>(serverItem->parent());
		if ( gameItem->curServerId() ) {
			;
		}else {
			requestLogin( gameItem, serverItem );
		}
		break;
	}
	default:
		djDebug() << "don't care this item type double click";
		break;
	}
}
void DJHallController::refreshGameItem( DJGameGameItem* gameItem )
{
	quint16 gameId	= gameItem->id();
	//get game icon,if installed
	QDir	d("modules");
	d.mkpath(".");
	QStringList files	= d.entryList(QDir::Files);
	bool isInstalled	= false;
	quint32 latestVersion	= 0;
	
	foreach( QString libFile, files ) {
		QString	libPath	= d.filePath( libFile );
		djDebug() << libPath;
		QLibrary lib( libPath );
				
		GIGetGameInfoProc	giGetGameInfoProc	= (GIGetGameInfoProc)(lib.resolve("GetGameInfo"));
		djDebug() << "giGetGameInfoProc" << giGetGameInfoProc;
		if ( giGetGameInfoProc ) {
			quint16 id;
			quint32	version;
			QIcon	icon;
			giGetGameInfoProc( id, version, icon );
			djDebug() << "id = "<< hex <<id << "version = " << version;
			
			if ( id == gameId && version > latestVersion ) {
				isInstalled	= true;
				QString	oldModuleName	= gameItem->libraryPath();
				gameItem->setLibraryPath( libPath );
				gameItem->setIcon( 0, icon );
				gameItem->setVersion( version );
				if ( 0 != latestVersion ) {
					//save old module name to delete
					djDebug() << "SetSettingOldModuleName";
					SetSettingOldModuleName( gameId, oldModuleName );
				}
			}
		}
	}
	
	QString	gameName	= gameItem->name();
	if ( !isInstalled ) {
		quint32 gameLastVersion	= GetSettingGameVersion(gameId);
		if ( 0 == gameLastVersion ) {
			gameItem->setText( 0,gameName + tr("(To be released)") );
		}else {
			gameItem->setText( 0,gameName + tr("(Not installed)") );
		}
	}else {
		gameItem->setText( 0,gameName );
	}
}
void DJHallController::addGameItem( quint16 gameId, const QString& gameName )
{		
	djDebug()<<"DJHallController::addGameItem"<<gameId<<gameName;
	
    DJGameClassItem	*gameClassItem	= 0;
    DJGameGameItem		*gameItem	= 0;
    DJGameServerItem	*serverItem	= 0;
    
    locateTreeItem( hibyte(gameId), 0, 0, gameClassItem, gameItem, serverItem );
	if ( gameClassItem ) {
		gameItem	= static_cast<DJGameGameItem *>(gameClassItem->childItem(gameId));
		if ( 0 == gameItem ) {
    		gameItem	= new DJGameGameItem(gameId,gameClassItem,DJGameItemGameType);
    	}
    	gameItem->setName( gameName );
    	refreshGameItem( gameItem );
    	m_gameTree->setItemExpanded( gameClassItem, true );
	}
}
void DJHallController::addServerItem( quint16 gameId, quint16 serverId, const QString& ip, quint16 port, 
		const QString& serverName, int numberOfUsers )
{	   
	djDebug()<<"addServerItem"<<gameId<<serverId<<ip<<port<<serverName<<numberOfUsers; 
    DJGameClassItem	*gameClassItem	= 0;
    DJGameGameItem		*gameItem	= 0;
    DJGameServerItem	*serverItem	= 0;
    
    locateTreeItem( hibyte(gameId), gameId, 0, gameClassItem, gameItem, serverItem );
	if ( gameItem) {
		serverItem	= static_cast<DJGameServerItem *>(gameItem->childItem(serverId));
		if ( 0 == serverItem ) {
			djDebug() << "serverItem == 0";
			serverItem	= new DJGameServerItem(serverId,gameItem,DJGameItemServerType);
		}
		serverItem->setIp(ip);
    	serverItem->setPort(port);
    	serverItem->setDescription( serverName, numberOfUsers );
    	m_gameTree->setItemExpanded( gameItem, true );
	}
}
void DJHallController::changeServerUsers( quint16 gameId, quint16 serverId, int numberOfServerUsers )
{
	DJGameClassItem	*gameClassItem	= 0;
    DJGameGameItem		*gameItem	= 0;
    DJGameServerItem	*serverItem	= 0;
    
    djDebug() << "DJHallController::changeServerUsers" << gameId << serverId << numberOfServerUsers;
    locateTreeItem( hibyte(gameId), gameId, serverId, gameClassItem, gameItem, serverItem );
    djDebug() << gameClassItem << gameItem << serverItem;
	if ( serverItem ) {
		djDebug() << "find server item";
		serverItem->setNumberOfUsers( numberOfServerUsers );
	}
}

void DJHallController::receiveChat( const ChatContent *chatContent, quint8 extraInfo )
{
	foreach( DJContact* contact, m_contacts ) {
		quint32 selfUserId	= contact->selfUserId();
		quint32	speakerId	= letoh4( &chatContent->userid_speaker );
		quint32	listenerId	= letoh4( &chatContent->userid_listener );
		if ( listenerId == selfUserId || speakerId == selfUserId ) {
			contact->receiveChat( chatContent, extraInfo );
			break;
		}
	}
}
void DJHallController::sendChat( quint32 speakerId, quint32 listenerId, quint16 gameId, const QString& contents )
{
	djDebug() << "DJHallController::sendChat"<<contents;
	QByteArray utf8	= contents.toUtf8();
	
	foreach( DJSocket *socket, m_gameSockets ) {
		if ( socket->containsGame( gameId ) ){
			
			QByteArray	chat(sizeof(ChatContent) + utf8.size()+1, 0);
			ChatContent	*pchat	= reinterpret_cast<ChatContent *>(chat.data());
			setle4( &pchat->userid_speaker, speakerId );
            setle4( &pchat->userid_listener, listenerId );
            pchat->chGameClass = hibyte(gameId);
            pchat->chGame = lobyte(gameId);
            qstrcpy( pchat->szSentence, utf8.data() );
            QByteArray	buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_CHAT, utf8.size(), m_language, chat );
            
            DJClientRequest request( buf );
			socket->sendRequest( request );
			break;
		}
	}
}
DJContact*	DJHallController::contact( QString selfUserName )
{
	djDebug() << "m_contacts.size = " << m_contacts.size();
	if ( m_contacts.contains( selfUserName ) )
		return m_contacts.value( selfUserName );
	else
		return 0;
}
void DJHallController::insertContact( QString selfUserName, DJContact* contact )
{
	m_contacts.insert( selfUserName, contact );
}
void DJHallController::removeContact( QString selfUserName )
{
	m_contacts.remove( selfUserName );
}
QVariant DJHallController::gameUserInfo( quint16 gameId, quint32 userId )
{
	DJGameClassItem	*gameClassItem	= 0;
    DJGameGameItem		*gameItem	= 0;
    DJGameServerItem	*serverItem	= 0;
    
    locateTreeItem( hibyte(gameId), gameId, 0, gameClassItem, gameItem, serverItem );
    if ( gameItem ) {
    	QVariant userInfo	= gameItem->gameControllerCommand( GAME_COMMAND_GAME_USER_INFO, userId );
    	return userInfo;
    }
    return QVariant();
}
void DJHallController::gameContext(int index)
{
	djDebug() << "DJHallController::gameContext" << index;
	
	m_gameContextMenu->clear();
	
	DJGameTab* tab	= static_cast<DJGameTab*>(m_tabContainer->widget(index));
	DJGameGameItem*	gameItem	= tab->gameItem();
	if ( gameItem ) {
    	QList<QVariant>	list;
		list << QVariant::fromValue(static_cast<void*>(m_gameContextMenu));
		QVariant	parameters(list);
    	gameItem->gameControllerCommand( GAME_COMMAND_GAME_CONTEXT, parameters );
	}else {
		//hall tab
		m_gameContextMenu->addAction( m_actQuitHall );
	}
	m_gameContextMenu->exec( QCursor::pos() );
}
void DJHallController::gameTreeContext( const QPoint &point )
{
	djDebug() << "gameTreeContext";
	
	m_gameContextMenu->clear();
	
	QTreeWidgetItem *treeItem	= m_gameTree->itemAt( point );
	if ( treeItem ) {
		switch( treeItem->type() ) {
		case DJGameItemGameType: {
			DJGameGameItem* gameItem	= static_cast<DJGameGameItem*>(treeItem);
			QVariant	data;
			data.setValue( static_cast<void *>(treeItem) );
			m_actRefreshServer->setData( data );
			m_gameContextMenu->addAction( m_actRefreshServer );
			
			djDebug()<<"gameItem version"<<gameItem->version();
			data.setValue( gameItem->version() );
			m_actQueryGameVersion->setData( data );
			m_gameContextMenu->addAction( m_actQueryGameVersion );
			
    		QList<QVariant>	list;
			list << QVariant::fromValue(static_cast<void*>(m_gameContextMenu));
			QVariant	parameters(list);
    		gameItem->gameControllerCommand( GAME_COMMAND_GAME_CONTEXT, parameters );
    		
			m_gameContextMenu->exec( QCursor::pos() );
			break;
		}
		default:
			break;
		}
	}else {
		m_gameContextMenu->addAction( m_actRefreshGame );
		m_gameContextMenu->addAction( m_actDownloadIni );
		m_gameContextMenu->exec( QCursor::pos() );
	}
}
void DJHallController::refreshGameList()
{
	djDebug() << "refreshGameList";
	
	getGameList();
}
void DJHallController::refreshServerList()
{
	djDebug() << "refreshServerList";
	
	void*	item	= m_actRefreshServer->data().value<void *>();
	DJGameGameItem	*gameItem	= static_cast<DJGameGameItem *>(item);
	
	getServerList( gameItem );
}
void DJHallController::queryGameVersion()
{
	djDebug() << "queryGameVersion";
	
	uint version	= m_actQueryGameVersion->data().toUInt();
	m_mainWindow->statusBar()->showMessage( QString::number(version,16) );
}
void DJHallController::requestGameList( const QString& ip, quint16 port )
{	
	djDebug() << "requestGameList" << ip << port;
	if ( QAbstractSocket::UnconnectedState != m_hallSocket->state() ) {
		QMessageBox::information( 0,tr("server is busy"),tr("please try it later"),QMessageBox::Ok);
		return;
	}

	QByteArray	buf;
	DJClientRequest	request1( buf, aclConnectedProc, QVariant::fromValue((void*)this) );
	m_hallSocket->appendRequest( request1 ); 
	
    buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_GETGAMELIST, 0, m_language );
    QVariant parameters	= QVariant::fromValue((void*)m_hallSocket);
	DJClientRequest request2( buf, aclRequestGameListProc, parameters );
	m_hallSocket->appendRequest( request2 );
	m_hallSocket->connectToServer( ip, port, true, tr("getting game list...") );
}
void DJHallController::requestServerList( const QString& ip, quint16 port, quint16 gameId )
{	
	djDebug() << "requestServerList" << "gameId = " << hex << gameId;

	if ( QAbstractSocket::UnconnectedState != m_hallSocket->state() ) {
		QMessageBox::information( 0,tr("server is busy"),tr("please try it later"),QMessageBox::Ok);
		return;
	}
    
	DJClientRequest	request1;
	m_hallSocket->appendRequest( request1 ); 
	
	QByteArray	id;
	id.append(gameId & 0xff);
	QByteArray	buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_GETSERVERLIST2, hibyte(gameId), m_language, id );
	QVariant parameters	= QVariant::fromValue((void*)m_hallSocket);
	DJClientRequest request2( buf, aclRequestServerListProc, parameters );
    m_hallSocket->appendRequest( request2 );
    
	m_hallSocket->connectToServer( ip, port, true, tr("getting server list...") );
}
/*
void DJHallController::findLoginNamePass( QString & userName, QString & password )
{
	userName.clear();
	password.clear();
	for ( int i = 0; i < m_gameTree->topLevelItemCount(); i++ ) {
		DJGameClassItem *classitem	= static_cast<DJGameClassItem *>(m_gameTree->topLevelItem(i));
		for ( int j = 0; j < classitem->childCount(); ++j ) {
			DJGameGameItem *gameItem	= static_cast<DJGameGameItem *>(classitem->child(j));
			QString name	= gameItem->loginName();
			QString pass	= gameItem->password();
			if ( !name.isEmpty() && !pass.isEmpty() ) {
				userName	= name;
				password	= pass;
				return;
			}
		}
	}
}
*/
void DJHallController::requestLogin( DJGameGameItem* gameItem, DJGameServerItem *serverItem )
{
	djDebug() << "requestLogin" << "serverItem = " << serverItem << "gameSocket count = " << m_gameSockets.count();
	djDebug() << "m_contacts.size = "<<m_contacts.size();
    quint16 gameId	= gameItem->id();
    	
	DJSocket	*liveSocket	= 0;
	DJSocket	*deadSocket	= 0;
	//QString userName,password;
	
	foreach( DJSocket* socket, m_gameSockets ) {
		djDebug() << "state = "<< socket->state();
		if ( socket->containsGame( gameId ) ) {
			//same game can't login twice
			QMessageBox::information( 0,tr("login error"),tr("you have logged in this game"),QMessageBox::Ok);
			return;
		}
		if ( QAbstractSocket::UnconnectedState == socket->state() ) {
			deadSocket	= socket;
		}else {
			//socket->getLoginNamePass( userName, password );
			//djDebug() << "username="<< userName<<"password="<< password;
    		if ( serverItem->ip() == socket->ip() && serverItem->port() == socket->port() ) {
    			//same socket,reuse it
    			liveSocket	= socket;
    			//gameItem->setLoginNamePass( userName, password );
    			break;
    		}
		}	
	}
    if ( liveSocket ) {
    	djDebug() << "find reuseable socket";
    	//send new game protocol

  	 	QByteArray	gid	= htoa2( gameId, QDataStream::BigEndian );
  	 	QByteArray	buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_NEWGAME, 0, m_language, gid );
        
        QList<QVariant>	list;
		list << QVariant::fromValue((void*)liveSocket) 
			<< QVariant::fromValue((void*)gameItem)
			<< QVariant::fromValue((void*)this);
		QVariant	parameters(list);
		
        DJClientRequest request( buf, aclLoginProc, parameters );
    	
    	liveSocket->insertGame( gameItem ,serverItem );
  	 	liveSocket->sendRequest( request );
  	 	
  	 	buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_GETCARDS, 0, m_language );
		DJClientRequest request3( buf );
		liveSocket->appendRequest( request3 );
    }else {
    	djDebug()<<"no live socket";
    	//djDebug() << "userName"<<userName << "same log"<<GetSettingSameLogin();
    	
    	/*
    	if ( GetSettingSameLogin() ) {
    		findLoginNamePass( userName, password );
    	}
    	if ( userName.isEmpty() ) {
			UILogin loginDlg;
        	if ( QDialog::Rejected == loginDlg.exec() )
				return;
			userName	= loginDlg.loginUserName();
			password	= loginDlg.loginPassword();
		}
		*/
		if ( m_loginName.isEmpty() ) {
			UILogin loginDlg;
        	if ( QDialog::Rejected == loginDlg.exec() )
				return;
			m_loginName	= loginDlg.loginUserName();
			m_loginPassword	= loginDlg.loginPassword();
		}
		//gameItem->setLoginNamePass( userName, password );
		
		DJSocket	*socket;
		if ( 0 == deadSocket ) {
			djDebug() << "no available socket,new one";
			socket	= new DJSocket( DJSocket::Game, this );
			m_gameSockets.append( socket );
		}else {
			//wakeup dead socket and use it
			djDebug() << "wake up dead socket";
			socket	= deadSocket;
		}
		QByteArray buf;
		DJClientRequest	request1( buf, aclConnectedProc, QVariant::fromValue((void*)this) );
		socket->appendRequest( request1 );
	
		QByteArray	login(sizeof(LOGININFO), 0);
		LOGININFO	*plogin	= reinterpret_cast<LOGININFO *>(login.data());
#if defined( Q_WS_MAC )
		plogin->chOS = DJGAME_CLIENT_OS_MACOS;
#elif defined( Q_WS_WIN )
		plogin->chOS = DJGAME_CLIENT_OS_WINDOWS;
#elif defined( Q_WS_X11 )
		QProcess uname;
    	uname.setReadChannelMode(QProcess::MergedChannels);
    	uname.start("uname", QStringList() << "-a");
		QByteArray	baOutput;
    	if (uname.waitForFinished(3000))
        	baOutput	= uname.readAll();
        QString strOutput(baOutput);
        if ( strOutput.contains("lemote",Qt::CaseInsensitive) )
        	plogin->chOS = DJGAME_CLIENT_OS_LOONGSON;
        else
			plogin->chOS = DJGAME_CLIENT_OS_LINUX;
#endif
		plogin->chGameClass	= hibyte(gameId);
        plogin->chGame		= lobyte(gameId);
        plogin->uLoginType 	= 1;
        qstrcpy((char *)plogin->szUserName,m_loginName.toUtf8());
		qstrcpy((char *)plogin->szPassword,m_loginPassword.toUtf8());
		
		buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_LOGIN, 0, m_language, login );
  	 	QList<QVariant>	list;
		list << QVariant::fromValue((void*)socket) 
			<< QVariant::fromValue((void*)gameItem)
			<< QVariant::fromValue((void*)this);
		QVariant	parameters(list);
	
    	DJClientRequest request2( buf, aclLoginProc, parameters );
    	socket->appendRequest( request2 );
		socket->insertGame( gameItem, serverItem );
	
		buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_GETCARDS, 0, m_language );
		DJClientRequest request3( buf );
		socket->appendRequest( request3 );
		
		socket->connectToServer( serverItem->ip(), serverItem->port(), true, tr("Logging in...") );
	}
}
void DJHallController::requestRelogin( DJGameGameItem* gameItem, DJSocket* socket )
{
	djDebug() << "requestRelogin"<<gameItem->id()<<(void*)socket<<socket->state();
	if ( m_isBanned )
		return;
	quint16 gameId	= gameItem->id();
	QString	ip	= socket->ip();
	quint16 port	= socket->port();
	djDebug()<<"ip="<<ip<<"port="<<port;
	if ( ip.isEmpty() || 0 == port )
		return;
		
	if ( QAbstractSocket::UnconnectedState == socket->state() ) {
		djDebug()<<"unconnected";
		DJClientRequest	request1;
    	socket->appendRequest( request1 ); 
    	
    	QByteArray	login(sizeof(LOGININFO), 0);
    	LOGININFO	*plogin	= reinterpret_cast<LOGININFO *>(login.data());
#if defined( Q_WS_MAC )
    	plogin->chOS = DJGAME_CLIENT_OS_MACOS;
#elif defined( Q_WS_WIN )
    	plogin->chOS = DJGAME_CLIENT_OS_WINDOWS;
#elif defined( Q_WS_X11 )
    	plogin->chOS = DJGAME_CLIENT_OS_LINUX;
#endif
    	plogin->chGameClass	= hibyte(gameId);
        plogin->chGame		= lobyte(gameId);
        plogin->uLoginType 	= 1;
        qstrcpy((char *)plogin->szUserName,m_loginName.toUtf8());
    	qstrcpy((char *)plogin->szPassword,m_loginPassword.toUtf8());
    	
    	QByteArray	buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_LOGIN, 0, m_language, login );
     	QList<QVariant>	list;
    	list << QVariant::fromValue((void*)socket) 
    		<< QVariant::fromValue((void*)gameItem)
    		<< QVariant::fromValue((void*)this);
    	QVariant	parameters(list);
    	DJClientRequest request2( buf, aclLoginProc, parameters );
    	socket->appendRequest( request2 );
    	
    	buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_GETCARDS, 0, m_language );
		DJClientRequest request3( buf );
		socket->appendRequest( request3 );
		
    	socket->connectToServer( ip, port, false );
    	gameItem->gameControllerCommand( GAME_COMMAND_RECONNECTING, QVariant() );

	}else if ( QAbstractSocket::ConnectedState == socket->state() ) {
		djDebug()<<"connected";
		QByteArray	gid	= htoa2( gameId, QDataStream::BigEndian );
  	 	QByteArray	buf	= MakeGeneralData(DJGAME_PROTOCOL_TYPE_NEWGAME, 0, m_language, gid );
        
        QList<QVariant>	list;
		list << QVariant::fromValue((void*)socket) 
			<< QVariant::fromValue((void*)gameItem)
			<< QVariant::fromValue((void*)this);
		QVariant	parameters(list);
		
        DJClientRequest request( buf, aclLoginProc, parameters );    	
  	 	socket->sendRequest( request );
  	 	
  	 	buf	= MakeGeneralData( DJGAME_PROTOCOL_TYPE_GETCARDS, 0, m_language );
		DJClientRequest request3( buf );
		socket->appendRequest( request3 );
		
		gameItem->gameControllerCommand( GAME_COMMAND_RECONNECTING, QVariant() );
	}
}
void DJHallController::locateTreeItem( uint gameClassId, uint gameId, uint serverId,
	DJGameClassItem * &gameClassItem, DJGameGameItem * &gameItem, DJGameServerItem * &serverItem )
{
	//djDebug() << "locateTreeItem" << gameClassId << gameId << serverId;
	gameClassItem	= 0;
	gameItem		= 0;
	serverItem		= 0;
	int i;
	
	//seach game class item if needed
	if ( 0 == gameClassId )
		return;
	for ( i = 0; i < m_gameTree->topLevelItemCount(); i++ ) {
		DJGameClassItem *item	= static_cast<DJGameClassItem *>(m_gameTree->topLevelItem(i));
		if ( gameClassId == item->id() ) {
			//find game class item
			gameClassItem	= item;
			break;
		}
	}
	//djDebug() << "find gameClassItem" << gameClassItem;
	
	//seach game item if needed
	if ( 0 == gameId || 0 == gameClassItem )
		return;
	else
		gameItem	= static_cast<DJGameGameItem *>(gameClassItem->childItem(gameId));
	djDebug() << "find gameItem" << gameItem;
	
	//seach server item if needed
	if ( 0 == serverId || 0 == gameItem )
		return;
	else 
		serverItem	= static_cast<DJGameServerItem *>(gameItem->childItem(serverId));
	//djDebug() << "find serverItem" << serverItem;
}

void DJHallController::getGameList()
{
	djDebug()<<"getGameList";
	//check if there is a new version hall
	quint32 hallIniVersion	= GetSettingHallVersion();
	int result	= CheckVersionMatch( hallIniVersion, HALL_VERSION );
	djDebug() <<hex<<"hallIniVersion"<<hallIniVersion<<"HALL_VERSION"<<HALL_VERSION;
	if ( result > 0 ) {
		//can update hall
		if ( 4 == result ) {
			if ( 0 == QMessageBox::information(0,tr("Update whole"),tr("The whole game need to reinstall, please download the latest whole install package at %1").arg(WWW_HOST_DOWNLOAD_URL),tr("Goto Download Page") ,tr("Later")) ) {
    			LaunchURL( WWW_HOST_DOWNLOAD_URL );
    		}
    		return;
		}else {
			if ( 0 == QMessageBox::information(0,tr("Update hall"),tr("The hall with higher version is available, do you wanna download now?"),tr("Download Now") ,tr("Later")) ) {
				downloadExecutable( DJDownloadManager::Hall, 0, UPDATE_INI_SECTION_HALL, tr("Hall") );
				return;
			}
		}
	}else if ( -4 == result && 0 != hallIniVersion && 0 == FirstVersion( hallIniVersion ) ) {
		//recycle
		if ( 0 == QMessageBox::information(0,tr("Update whole"),tr("The whole game need to reinstall, please download the latest whole install package at %1").arg(WWW_HOST_DOWNLOAD_URL),tr("Goto Download Page") ,tr("Later")) ) {
    		LaunchURL( WWW_HOST_DOWNLOAD_URL );
    	}
    	return;
	}

	QString ip;
	quint16	port;
	GetSettingLastServer( ip, port );
	requestGameList( ip, port );
}

void DJHallController::getServerList( DJGameGameItem* gameItem )
{
	QString	libPath	= gameItem->libraryPath();
	quint16	gameId	= gameItem->id();
	QString	name	= gameItem->name();
	djDebug() << "GetServerList" << libPath << hex << gameId;
	//if ( QLibrary::isLibrary( libPath ) ) {
	if ( !libPath.isEmpty() ) {
		quint32 gameIniVersion	= GetSettingGameVersion( gameId );
		quint32 gameVersion	= gameItem->version();
		djDebug()<<hex<<"gameIniVersion"<<gameIniVersion<<"gameVersion"<<gameVersion;
		int result	= CheckVersionMatch( gameIniVersion, gameVersion );
		djDebug()<<"game result="<<result;
		if ( result > 0 ) {
			//can update game
			if ( 0 == QMessageBox::information(0,tr("Update game"),tr("The game with higher version is available, do you wanna download now?"),tr("Download Now") ,tr("Later"))) {
        		downloadExecutable( DJDownloadManager::Library, gameId, QString::number(gameId,16), gameItem->name() );
        		return;
        	}
		}
		//check if hall and base version match to game
		result	= CheckVersionMatch( m_whbVersion, gameVersion & 0xFFFFFF00 );
		djDebug()<<"hallbase game result="<<hex<<m_whbVersion<<gameVersion<<result;
		if ( result > 0 ) {
			//must update game
			if ( 0 == QMessageBox::information(0,tr("Update game"),tr("Current version of %1 is too low to be loaded\n").arg(name) +  tr("please download the latest version"),tr("Download Now") ,tr("Cancel")) ) {
				downloadExecutable( DJDownloadManager::Library, gameId, QString::number(gameId,16), name );
			}
		}else if ( result < 0 ) {
			//must update whole or hall or base
			switch(result) {
			case -4:
				if ( 0 == QMessageBox::information(0,tr("Update whole"),tr("The whole game need to reinstall, please download the latest whole install package at %1").arg(WWW_HOST_DOWNLOAD_URL),tr("Goto Download Page") ,tr("Later")) ) {
					LaunchURL( WWW_HOST_DOWNLOAD_URL );
				}
				break;
			case -3:
				name	= tr("Hall");
				if ( 0 == QMessageBox::information(0,tr("Update hall"),tr("Current version of %1 is too low to load this game\n").arg(name) +  tr("please download the latest version"),tr("Download Now") ,tr("Cancel")) ) {
    				downloadExecutable( DJDownloadManager::Hall, 0, UPDATE_INI_SECTION_HALL, name );
    			}
				break;
			case -2:
				name	= tr("Base");
				if ( 0 == QMessageBox::information(0,tr("Update base"),tr("Current version of %1 is too low to load this game\n").arg(name) +  tr("please download the latest version"),tr("Download Now") ,tr("Cancel")) ) {
    				downloadExecutable( DJDownloadManager::Base, 0, UPDATE_INI_SECTION_BASE, name );
				}
				break;
			default:
				break;
			}
		}else {
			//hall base verison match game version, we can go on now
			QString ip;
			quint16	port;
			GetSettingLastServer( ip, port );
			requestServerList( ip, port, gameId );
		}
	}else {
		//game is not installed, download it
		if ( 0 == QMessageBox::information(0,tr("Install game"),tr("You haven't installed this game, do you wanna download now?"),tr("Download Now") ,tr("Later")) ) {
			djDebug() << "download new game";
			downloadExecutable( DJDownloadManager::Library, gameId, QString::number(gameId,16), gameItem->name() );
    	}
	}
}
void DJHallController::downloadExecutable( DJDownloadManager::FileType type, quint16 id, const QString& section, const QString& name )
{
	if ( m_downloadManger->isDownloading() ) {
		QMessageBox::information(0,tr("Download failed"),tr("Download service is busy\n") + tr("Please try it later(%1)").arg(name),QMessageBox::Ok);
		return;
	}
	UISelectDownloadServer sdsDlg;
	/*
	QString	section;
	if ( 0 == id )
		section	= "Hall";
	else
		section	= QString::number(id,16);
		*/
	if ( sdsDlg.selectDownloadServer(section) ) {
		if ( QDialog::Accepted == sdsDlg.exec() ) {
			QString url	= sdsDlg.url();
			m_downloadManger->download( url, type, id, name, sdsDlg.resume() );
		}
	}else {
		QMessageBox::information(0,tr("Download failed"),tr("Sorry, no downloadable file temporarily"),QMessageBox::Ok);
	}
}
void DJHallController::downloadIni()
{
	djDebug() << "downloadIni()";
	QString name	= tr("Ini");
	if ( m_downloadManger->isDownloading() ) {
		QMessageBox::information(0,tr("Download failed"),tr("Download service is busy\n") + tr("Please try it later(%1)").arg(name),QMessageBox::Ok);
		return;
	}
	m_downloadManger->download( GetSettingUpdateIniUrl(), DJDownloadManager::Ini, 0, name, false, true );
}
void DJHallController::downloadResource()
{
	djDebug() << "DJHallController::downloadResource";
	
	if ( m_downloadManger->isDownloading() ) {
		return;
	}
	QHash<QString, QString>::iterator it = m_resources.begin();
	while ( it != m_resources.end() ) {
        QString	localFile	= it.key();
        QString remoteUrl	= it.value();
        qint64 remoteFileSize	= remoteUrl.section("/",-1).section(".",-1).toLongLong();
        djDebug() << "remoteFileSize = " << remoteFileSize;
        QFileInfo localFileInfo(localFile);
        //localFileInfo.exists()
		if (  remoteFileSize == localFileInfo.size() ) {
			it	= m_resources.erase(it);
		}else {
			m_resources.remove(localFile);
        	m_downloadManger->download( remoteUrl, DJDownloadManager::Resource, 0, tr("resource file"), false, true, localFile );
        	return;
        }
    }
}
void DJHallController::downloadLogin()
{
#if QT_VERSION < 0x040100 && defined( Q_WS_WIN )
	QString digitalHost;
	QHostInfo info	= QHostInfo::fromName("10.0.0.153");
	if ( !info.addresses().isEmpty() ) {
		digitalHost = info.addresses().first().toString();
	}
	m_http->setHost( digitalHost );
#else	
	m_http->setHost( "10.0.0.153" );
#endif

	m_http->get( QString(AUTH_LOGIN_PATH).arg(m_loginName).arg(m_loginPassword),m_httpFile);
	
	connect(m_http, SIGNAL(done(bool)), SLOT(loginDone(bool)) );
}
void DJHallController::loginDone( bool error )
{
	djDebug()<<"DJHallController::httpdone"<<error;
	djDebug()<<m_http->errorString();
	bool login	= false;
	
	if ( !error ) {
		//if ( m_httpFile->open( QIODevice::ReadOnly ) ) {
			QByteArray buf	= m_httpFile->readAll();
			DebugBuffer( buf );
			if ( buf.startsWith("OK") ) {
				login	= true;
			}
		//}
	}
	if ( !login ) {
		launchAppFinished();
	}else {
		//check if there is unfinished update for hall
    	QDir hallDir("update/hall");
    	QStringList hallFiles	= hallDir.entryList(QDir::Files);
    	if ( !hallFiles.isEmpty() ) {
    		if ( 0 == QMessageBox::information(0,tr("Hall update is unfinish"),tr("go on to finish"),tr("Go on") ,tr("Later")) ) {
    			RelaunchHallForUpdate();
    			return;
    		}
    	}
    	//check if there is unfinished update for base
    	QDir baseDir("update/base");
    	QStringList baseFiles	= baseDir.entryList(QDir::Files);
    	for( int i = 0; i < baseFiles.size(); i++ ) {
    		QString fileName	= baseFiles.at(i);
    		djDebug() << fileName;
    		QString sourceFilePath	= baseDir.filePath( fileName );
    		QString targetFilePath	= QString("./") + fileName;
    		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
    			;
    		}else {
    			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
    		}
    	}
    	
    	DeleteAllOldModules();
    	ReviseModuleName();
    	
    	loadBaseLibrary();
    	
    	//check if download ini file for update
    	if ( iniExpired() ) {
    		downloadIni();
    	}
    	
    	getGameList();
	}
}
/*
void DJHallController::downloadHallUpdator()
{
	djDebug() << "DJHallController::downloadHallUpdator";
	
	if ( m_downloadManger->isDownloading() )
		return;
	int		fileSize;
	QString	fileName;
	
	GetSettingRemoteFileSizeName( KEY_HALL_UPDATOR, fileSize, fileName );
	if ( 0 == fileSize || fileName.isEmpty() )
		return;
	
	djDebug() << "fileSize = " << fileSize << "fileName =" << fileName;

	QFileInfo localFileInfo;
#if defined( Q_WS_MAC )	
	localFileInfo.setFile("UpdateHall.app/Contents/MacOS/UpdateHall");
#elif defined( Q_WS_WIN )
	localFileInfo.setFile("UpdateHall.exe");
#elif defined( Q_WS_X11 )
	localFileInfo.setFile("UpdateHall");
#endif

	QString displayName	= tr("HallUpdator");
	if ( fileSize == localFileInfo.size() )
		return;
	else {
		QMessageBox::information(0,tr("Update %1").arg(displayName),tr("New %1 will be downloaded").arg(displayName),QMessageBox::Ok);
	}
	UISelectDownloadServer sdsDlg;
	if ( sdsDlg.selectDownloadServer(KEY_HALL_UPDATOR) ) {
		if ( QDialog::Accepted == sdsDlg.exec() ) {
			QString url	= sdsDlg.url();
			m_downloadManger->download( url, DJDownloadManager::HallUpdator, 0, displayName, sdsDlg.resume(), false, localFileInfo.filePath() );
		}
	}else {
		QMessageBox::information(0,tr("Download %1 failed").arg(displayName),tr("Sorry, no downloadable file temporarily"),QMessageBox::Ok);
	}
}
*/
void DJHallController::downloadFinished( QFile* file, const QString& localFile, int fileType, quint16 id, const QString& name, int lastStatusCode )
{
	QString	sourceFilePath	= file->fileName();
	djDebug() << "downloadFinished" << sourceFilePath << localFile << fileType << id << name << lastStatusCode;
	if ( sourceFilePath.isEmpty() ) {
		if ( /*0 != lastStatusCode &&*/ 200 != lastStatusCode && 206 != lastStatusCode && 
			( /*DJDownloadManager::Ini == fileType ||*/ DJDownloadManager::Hall == fileType || DJDownloadManager::Library == fileType ) )
			QMessageBox::information(0,tr("Download"),tr("Can't download %1,please try other server").arg(name),QMessageBox::Ok);
		return;
	}
	djDebug() << "********************";
	switch( fileType ) {
	case DJDownloadManager::Ini : {//ini
		djDebug() << "ini download ok";
		QString targetFilePath	= "update/Update.ini";
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			uint currentDate	= QDateTime::currentDateTime().toTime_t();
			djDebug() << "currentDate" << currentDate;
			SetSettingLastUpdate( QVariant(currentDate) );
			GetSettingResource( m_resources );
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
		break;
	}
	case DJDownloadManager::Hall : {//hall
		djDebug() << "hall download ok";
		QString targetFileName	= file->fileName().section("/",-1).section(".",0,-2);
		QString targetFilePath	= QString("update/hall/") + targetFileName;
		djDebug() << targetFileName << targetFilePath;
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			if ( 0 == QMessageBox::information(0,tr("Download %1 Finished").arg(name),tr("Relaunch the hall to finish update."),tr("Relaunch Now") ,tr("Cancel")) ) {
				RelaunchHallForUpdate();
			}
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
		break;
	}
	case DJDownloadManager::Library : {//library
		djDebug() << "library download ok";
		QString targetFileName	= file->fileName().section('/',-1);
		QString targetFilePath	= QString("modules/") + targetFileName;
		djDebug() << targetFileName << targetFilePath;
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			DJGameClassItem	*gameClassItem	= 0;
        	DJGameGameItem		*gameItem	= 0;
        	DJGameServerItem	*serverItem	= 0;
            
        	locateTreeItem( hibyte(id), id, 0, gameClassItem, gameItem, serverItem );
       		if ( gameItem ) {
        		refreshGameItem( gameItem );
        		QMessageBox::information(0,tr("Download %1 Finished").arg(name),tr("You can start game now"),QMessageBox::Ok );
        	}
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
		break;
	}
	case DJDownloadManager::Resource : {//resource
		djDebug() << "resource download ok";
		QString targetFilePath	= localFile;
		djDebug() << targetFilePath;
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			;
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
		break;
	}
	case DJDownloadManager::Base : {//base
		djDebug() << "base download ok";
		QString targetFileName	= file->fileName().section("/",-1).section(".",0,-2);
		QString targetFilePath	= QString("update/base/") + targetFileName;
		djDebug() << targetFileName << targetFilePath;
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			//go on copy update/base/base.lib to hall
			//sourceFilePath	= targetFilePath;
			//targetFilePath	= QString("./") + targetFileName;
			//DJMoveFile( sourceFilePath, targetFilePath );
			if ( 0 == QMessageBox::information(0,tr("Download %1 Finished").arg(name),tr("You need to relaunch the hall to finish update."),tr("Relaunch Now") ,tr("Cancel")) ) {
				RelaunchHallForUpdate();
			}			
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
		break;
	}
	case DJDownloadManager::HallUpdator : {//HallUpdator
		djDebug() << "HallUpdator download ok";
		/*
		QString targetFilePath	= localFile;
		djDebug() << targetFilePath;
		if ( DJMoveFile( sourceFilePath, targetFilePath ) ) {
			QMessageBox::information(0,tr("Update %1 finished").arg(name),tr("You have updated the hall updator"),QMessageBox::Ok);
		}else {
			QMessageBox::information(0,tr("Copy file error"),tr("src:%1\ndst:%2").arg(sourceFilePath).arg(targetFilePath),QMessageBox::Ok);
		}
		*/
	}
	default:
		break;
	}
}

