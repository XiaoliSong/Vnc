#include"login_dialog.h"

Login_dialog::Login_dialog(QWidget *parent) :QDialog(parent)
{
	id_edit = new QLineEdit(this);
	id_edit->setPlaceholderText(QString("�˺�"));
	id_edit->setStyleSheet("max-width:200px;height:30px;");

	pw_edit = new QLineEdit(this);
	pw_edit->setEchoMode(QLineEdit::Password);
	pw_edit->setPlaceholderText(QString("����"));
	pw_edit->setStyleSheet("max-width:200px;height:30px;");

	login_btn = new QPushButton(this);
	login_btn->setObjectName(QStringLiteral("login_btn"));
	login_btn->setText(QString("��¼"));
	login_btn->setStyleSheet("max-width:100px;height:30px;");
	login_btn->setDisabled(true);

	remmber_btn = new QRadioButton(this);
	remmber_btn->setText(QString("��ס����"));

	noti_label = new QLabel(this);
	noti_label->setText(get_color_string("δ���ӷ�����","red"));

	QGridLayout *layout = new QGridLayout(this);
	layout->setSpacing(30);
	layout->setAlignment(Qt::AlignCenter);
	layout->addWidget(id_edit, 0, 0, 1, 4);
	layout->addWidget(pw_edit, 1, 0, 1, 4);
	layout->addWidget(login_btn, 3, 1, 1, 2);
	layout->addWidget(remmber_btn, 4, 1, 1, 2);
	layout->addWidget(noti_label, 5, 1, 1, 2);

	this->setLayout(layout);
	this->setMinimumSize(400, 280);
	this->setWindowTitle(QString("��¼"));
	this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);

	connect(login_btn, &QPushButton::clicked, this, &Login_dialog::login);
}

void Login_dialog::read_config()
{
	QSettings *config_ini_read = new QSettings("config.ini", QSettings::IniFormat);
	if (config_ini_read->status() != 0) {
		QMessageBox box(QMessageBox::Warning, "ʧ��", "��ȡ�����ļ�config.iniʧ�ܣ�");
		box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		box.setButtonText(QMessageBox::Ok, QString("ȷ ��"));
		box.setButtonText(QMessageBox::Cancel, QString("ȡ ��"));
		box.exec();
		delete config_ini_read;
		exit(0);
	}
	address = config_ini_read->value("/server/address").toString();
	msg_ws_port = config_ini_read->value("/server/msg_port").toString();
	vnc_ws_port = config_ini_read->value("/server/vnc_port").toString();
	id = config_ini_read->value("/user/id").toString();
	pw = config_ini_read->value("/user/pw").toString();
	delete config_ini_read;

	if (address == "" || msg_ws_port == "" || vnc_ws_port == "") {
		QMessageBox box(QMessageBox::Warning, "ʧ��", "�����ļ�config.ini��server�γ���");
		box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		box.setButtonText(QMessageBox::Ok, QString("ȷ ��"));
		box.setButtonText(QMessageBox::Cancel, QString("ȡ ��"));
		box.exec();
		exit(0);
	}

	msg_ws_url = QString("ws://").append(address).append(":").append(msg_ws_port);
	vnc_ws_url= QString("ws://").append(address).append(":").append(vnc_ws_port);

	web_url = "http://";
	web_url = web_url.append(address);

	id_edit->setText(id);
	pw_edit->setText(pw);
	if (pw == "") {
		remmber_btn->setChecked(false);
	}
	else {
		remmber_btn->setChecked(true);
	}
	qInfo("read config successfully");
}

void Login_dialog::write_config()
{
	QSettings *config_ini_write = new QSettings("config.ini", QSettings::IniFormat);
	if (config_ini_write->status() != 0) {
		QMessageBox box(QMessageBox::Warning, "ʧ��", "д�������ļ�config.iniʧ�ܣ�");
		box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		box.setButtonText(QMessageBox::Ok, QString("ȷ ��"));
		box.setButtonText(QMessageBox::Cancel, QString("ȡ ��"));
		box.exec();
		delete config_ini_write;
		return;
	}
	
	config_ini_write->setValue("/server/address",address.toStdString().c_str());
	config_ini_write->setValue("/server/msg_port",msg_ws_port.toStdString().c_str());
	config_ini_write->setValue("/server/vnc_port", vnc_ws_port.toStdString().c_str());
	config_ini_write->setValue("/user/id",id.toStdString().c_str());
	if (remmber_btn->isChecked()) {
		config_ini_write->setValue("/user/pw",pw.toStdString().c_str());
	}
	else {
		config_ini_write->setValue("/user/pw", QString("").toStdString().c_str());
	}
	delete config_ini_write;
	qInfo("wirte config successfully");
}

void Login_dialog::login()
{
	if (pw != pw_edit->text()) {
		QByteArray hash = QCryptographicHash::hash(pw_edit->text().toLatin1(),QCryptographicHash::Sha1);
		pw = hash.toHex();
	}

	if (id != id_edit->text()) {
		id = id_edit->text();
	}

	if (id.isEmpty() || pw.isEmpty()) {
		QMessageBox::information(this, QString("�������"), QString("�˺ź����벻��Ϊ�գ�"), QMessageBox::NoButton);
	}

	write_config();
	emit(to_login(id, pw));
	qInfo("login is called successfully");
}


void Login_dialog::on_msg_client(QString msg)
{
	if (get_fun(msg) == "connect") {
		if (get_content(msg) == "pass") {
			login_btn->setEnabled(true);
			noti_label->setText(get_color_string("�������Ϸ�����", "green"));
			qInfo("success:msg_client connected to the server");
		}
		else {
			noti_label->setText(get_color_string("���ӷ�����ʧ��", "red"));
			qInfo("error:msg_client can't connect to the server");
		}
		return;
	}

	if (get_fun(msg) == "login") {
		if (get_content(msg) == "pass") {
			noti_label->setText(get_color_string("��¼�ɹ�", "green"));

			accept();
			App app(0);
			app.set_info(id, pw, web_url);

			Vnc_app vnc_app(0);
			vnc_app.connect_login(id, pw, vnc_ws_url);

			QObject::connect(&app, &App::run_stop, &vnc_app, &Vnc_app::onRunStop);
			QObject::connect(&vnc_app, &Vnc_app::to_app, &app, &App::on_vnc_app);
			QObject::connect(p_msg_client, &Msg_client::to_app, &app, &App::on_msg_client);
			QObject::connect(&app, &App::to_msg_client, p_msg_client, &Msg_client::send_msg);
			
			app.timer->start(500);
			app.exec();
			return;
		}
		if (get_content(msg) == "error") {
			noti_label->setText(get_color_string("��¼ʧ�ܣ������ʺ�����", "red"));

			QMessageBox box(QMessageBox::Warning, "ʧ��", "��¼ʧ�ܣ������ʺ����룡");
			box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			box.setButtonText(QMessageBox::Ok, QString("ȷ ��"));
			box.setButtonText(QMessageBox::Cancel, QString("ȡ ��"));
			box.exec();
			return;
		}

		if (get_content(msg) == "again") {
			noti_label->setText(get_color_string("��¼ʧ�ܣ��Ѿ��������ط���¼", "red"));
			QMessageBox box(QMessageBox::Warning, "ʧ��", "��¼ʧ�ܣ��Ѿ��������ط���¼��");
			box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
			box.setButtonText(QMessageBox::Ok, QString("ȷ ��"));
			box.setButtonText(QMessageBox::Cancel, QString("ȡ ��"));
			box.exec();
			return;
		}
		qInfo("error:login msg");
	}
}
