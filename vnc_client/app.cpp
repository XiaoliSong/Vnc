#include<app.h>

App::App(QWidget *parent) :QDialog(parent)
{
	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &App::handleTimeout);

	to_public_file_btn = new QPushButton;
	to_public_file_btn->setText(QString("�����ļ�"));
	to_public_file_btn->setStyleSheet("height:30px;width:50px");

	to_person_file_btn = new QPushButton;
	to_person_file_btn->setText(QString("�����ļ�"));
	to_person_file_btn->setStyleSheet("height:30px;width:50px");

	to_change_info_btn = new QPushButton;
	to_change_info_btn->setText(QString("�޸���Ϣ"));
	to_change_info_btn->setStyleSheet("height:30px;width:50px");

	to_run_stop_btn = new QPushButton;
	to_run_stop_btn->setText(QString("��ʼ������Ļ"));
	to_run_stop_btn->setStyleSheet("height:30px;width:100px");
	to_run_stop_btn->setDisabled(true);

	online_table = new QTableWidget(0,1);
	online_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	online_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	online_table->setSelectionBehavior(QAbstractItemView::SelectItems);
	online_table->horizontalHeader()->setVisible(false);

	allow_any_btn =new QRadioButton(this);
	allow_any_btn->setText(QString("�����κ��˲鿴"));
	allow_any_btn->setChecked(false);

	QStringList header;
	header << "�ʺ�" << "����";
	online_table->setHorizontalHeaderLabels(header);

	noti_text = new QTextEdit(this);
	noti_text->setStyleSheet("color:red;");
	noti_text->setAlignment(Qt::AlignLeft);
	noti_text->setDisabled(true);
	noti_text->setFixedHeight(100);
	QGridLayout *layout = new QGridLayout(this);

	layout->addWidget(to_public_file_btn, 0, 2, 1, 2);
	layout->addWidget(to_person_file_btn, 0, 5, 1, 2);
	layout->addWidget(to_change_info_btn, 0, 8, 1, 2);//8+2+2
	layout->addWidget(online_table,1,1,10,10);
	layout->addWidget(to_run_stop_btn, 12, 5, 1, 2);
	layout->addWidget(noti_text, 13, 1, 1, 10);
	layout->addWidget(allow_any_btn, 14, 5, 1, 2);

	//layout->setColumnStretch(0, 1);
//	layout->setColumnStretch(12, 1);
	// ����ˮƽ���
	layout->setHorizontalSpacing(10);
	// ���ô�ֱ���
	layout->setVerticalSpacing(10);
	// ��������
	layout->setContentsMargins(10, 10, 10, 10);

	this->setLayout(layout);
	this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
	this->setMinimumWidth(500);
	this->setMinimumHeight(600);

	connect(to_public_file_btn, &QPushButton::clicked, this, &App::to_public_file);
	connect(to_person_file_btn, &QPushButton::clicked, this, &App::to_person_file);
	connect(to_change_info_btn, &QPushButton::clicked, this, &App::to_change_info);
	connect(to_run_stop_btn, &QPushButton::clicked, this, &App::to_run_stop);
}


void App::set_info(QString id, QString pw, QString vnc_url)
{
	this->id = id;
	QString url("");
	url = vnc_url.remove("ws://");
	url = url.left(url.indexOf(":"));
	url.prepend("http://");

	this->to_change_info_url= QString("/file_manager/changepw.html?id=").append(id).append("&pw=").append(pw);
	this->to_person_fie_url= QString("/file_manager/file_person.html?id=").append(id).append("&pw=").append(pw).append("&to_see_id=").append(id);
	this->to_public_file_url = QString("/file_manager/file_public.html?id=").append(id).append("&pw=").append(pw);
	this->vnc_url= QString("/viewer/view.html?id=").append(id).append("&pw=").append(pw);

	this->to_change_info_url.prepend(url);
	this->to_person_fie_url.prepend(url);
	this->to_public_file_url.prepend(url);
	this->vnc_url.prepend(url);

	this->setWindowTitle(id + " ������");
}


void App::to_public_file()
{
	QDesktopServices::openUrl(to_public_file_url);

}


void App::to_person_file()
{
	QDesktopServices::openUrl(to_person_fie_url);
}


void App::to_change_info()
{
	QDesktopServices::openUrl(to_change_info_url);
}


void App::to_run_stop()
{
	if (this->is_run == true){
		this->is_run = false;
		this->to_run_stop_btn->setText("��ʼ������Ļ");
		noti_text->append("�Ѿ�ֹͣ������Ļ");
	}
	else {
		this->is_run = true;
		this->to_run_stop_btn->setText("ֹͣ������Ļ");
		noti_text->append("�Ѿ���ʼ������Ļ");
	}
	emit(run_stop(this->is_run));
	
}


void App::on_vnc_app(QString msg)
{
	if (get_fun(msg) == "connect") {
		if (get_content(msg) == "pass") {
			noti_text->append("�������Ϸ�����");
		}
		else {
			noti_text->append("���ӷ�����ʧ��");
			to_run_stop_btn->setDisabled(true);
		}
		return;
	}

	if (get_fun(msg) == "login") {
		if (get_content(msg) == "pass") {
			to_run_stop_btn->setEnabled(true);
			noti_text->append("��¼�ɹ�");
		}
		else {
			noti_text->setText("��¼ʧ�ܣ������ʺ�����");
		}
		return;
	}
}



void App::add_member(QString id)
{
	online_table->setRowCount(online_table->rowCount() + 1);
	online_table->setCellWidget(online_table->rowCount() - 1, 0, new Member(id,vnc_url, 0));
	online_table->setRowHeight(online_table->rowCount() - 1, 50);
	QObject::connect((Member *)online_table->cellWidget(online_table->rowCount() - 1, 0), &Member::send_msg, this, &App::on_member);
	noti_text->append(id+(" ������"));
}

void App::delete_member(QString id, int i)
{
	online_table->removeRow(i);
	noti_text->append(id+(" ������"));
}


void  App::on_msg_client(QString msg)
{
	if (get_fun(msg) == "signal") {
		msg = get_content(msg);
		QString temp_id = get_fun(msg);
		int index = online_list.indexOf(temp_id);
		if (index != -1) {
			Member *menber = (Member *)online_table->cellWidget(index, 0);
			QString content= get_content(msg);

			if (content == "ask_view") {
				noti_text->append(temp_id+(" ����鿴�������"));

				if (allow_any_btn->isChecked()) {
					emit(to_msg_client("signal;" + temp_id + ";pass_view"));
					noti_text->append(" ���Զ�ͬ�� " + temp_id + " �Ĳ鿴��������");
					try {
						menber->is_push_view = true;
						menber->push_view_btn->setText(QString("�رղ鿴"));
					}
					catch (exception& e) {

					}
				}
				else {
					QMessageBox box(QMessageBox::Question, "����鿴", temp_id + (" ����鿴�������"));
					box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
					box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
					box.setButtonText(QMessageBox::No, QString("ȡ ��"));
					int nRet = box.exec();
					if (QMessageBox::Yes == nRet) {
						emit(to_msg_client("signal;" + temp_id + ";pass_view"));
						noti_text->append(" ��ͬ�� " + temp_id + " �Ĳ鿴��������");
						try {
							menber->is_push_view = true;
							menber->push_view_btn->setText(QString("�رղ鿴"));
						}
						catch (exception& e) {

						}
					}

					if (QMessageBox::No == nRet) {
						emit(to_msg_client("signal;" + temp_id + ";deny_view"));
						noti_text->append(" �Ѿܾ� " + temp_id + " �Ĳ鿴��������");
					}
				}
				return;
			}

			if (content == "ask_control") {
				noti_text->append(temp_id+(" ��������������"));

				QMessageBox box(QMessageBox::Question, "�������", temp_id+(" ��������������"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					emit(to_msg_client("signal;" + temp_id + ";pass_control"));
					noti_text->append(" ��ͬ�� " + temp_id + " �Ŀ�����������");
					try {
						menber->is_push_view = true;
						menber->push_view_btn->setText(QString("�رղ鿴"));
						menber->is_push_control = true;
						menber->push_control_btn->setText(QString("�رտ���"));
					}
					catch (exception& e) {

					}
				}

				if (QMessageBox::No == nRet) {
					emit(to_msg_client("signal;" + temp_id + ";deny_control"));
					noti_text->append(" �Ѿܾ� " + temp_id + " �Ŀ�����������");
				}
				return;
			}

			if (content == "push_view") {
				try {
					menber->recv_msg(content);
				}
				catch(exception& e){

				}
				noti_text->append(temp_id+(" ��������������鿴"));

				QMessageBox box(QMessageBox::Information, "��������鿴", temp_id+(" ��������������鿴���򿪶Է�����?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					QDesktopServices::openUrl(vnc_url + "&to_see_id=" + temp_id + "&way=view");
				}
				return;
			}

			if (content == "push_control") {
				try {
					menber->recv_msg(content);
				}
				catch (exception& e) {

				}
				noti_text->append(temp_id+(" �����������������"));

				QMessageBox box(QMessageBox::Information, "�����������", temp_id+(" ����������������ƣ����ƶԷ�����?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					QDesktopServices::openUrl(vnc_url + "&to_see_id=" + temp_id + "&way=control");
				}
				return;
			}

			if (content == "pass_view") {
				try {
					menber->recv_msg(content);
				}
				catch (exception& e) {

				}
				noti_text->append(temp_id+(" ��ͬ����Ĳ鿴��������"));

				QMessageBox box(QMessageBox::Information, "ͬ��鿴", temp_id+(" ��ͬ����Ĳ鿴�������󣬴򿪶Է�����?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					QDesktopServices::openUrl(vnc_url + "&to_see_id=" + temp_id + "&way=view");
				}
				return;
			}

			if (content == "pass_control") {
				try {
					menber->recv_msg(content);
				}
				catch (exception& e) {

				}
				noti_text->append(temp_id+(" ��ͬ����Ŀ�����������"));

				QMessageBox box(QMessageBox::Information, "ͬ�����", temp_id+(" ��ͬ���������������ƣ����ƶԷ�����?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					QDesktopServices::openUrl(vnc_url + "&to_see_id=" + temp_id + "&way=control");
				}
				return;
			}

			if (content == "deny_view") {
				noti_text->append(temp_id+(" �Ѿܾ���Ĳ鿴��������"));

				QMessageBox box(QMessageBox::Information, "�ܾ��鿴", temp_id+(" �Ѿܾ���Ĳ鿴�����������·�������?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					emit(to_msg_client("signal;" + temp_id + ";ask_view"));
					noti_text->append(" ������ " + temp_id + " ���Ͳ鿴��������");
				}
			}

			if (content == "deny_control") {
				noti_text->append(temp_id+(" �Ѿܾ���Ŀ�����������"));

				QMessageBox box(QMessageBox::Information, "�ܾ�����", temp_id+(" �Ѿܾ��������������ƣ����·�������?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					emit(to_msg_client("signal;" + temp_id + ";ask_control"));
					noti_text->append(" ������ " + temp_id + " ���Ϳ�����������");
				}
				return;
			}

			if (content == "close_view") {
				try {
					menber->recv_msg(content);
				}
				catch (exception& e) {

				}
				noti_text->append(temp_id+(" �ѹر��������鿴"));

				QMessageBox box(QMessageBox::Information, "�رղ鿴", temp_id+(" �ѹر��������鿴�����·�������?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					emit(to_msg_client("signal;" + temp_id + ";ask_view"));
				}
				return;
			}

			if (content == "close_control") {
				try {
					menber->recv_msg(content);
				}
				catch (exception& e) {

				}
				noti_text->append(temp_id+(" �ѹر�����������"));

				QMessageBox box(QMessageBox::Information, "�رտ���", temp_id+(" �ѹر����������ƣ����·�������?"));
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setButtonText(QMessageBox::Yes, QString("ȷ ��"));
				box.setButtonText(QMessageBox::No, QString("ȡ ��"));
				int nRet = box.exec();
				if (QMessageBox::Yes == nRet) {
					emit(to_msg_client("signal;" + temp_id + ";ask_control"));
				}
				return;
			}

		}
		return;
	}

	if (get_fun(msg) == "newer") {
		QString temp_id = get_content(msg);
		if (online_list.indexOf(temp_id) == -1 && temp_id != id) {
			online_list.append(temp_id);
			add_member(temp_id);
		}
		return;
	}

	if (get_fun(msg) == "outer") {
		QString temp_id = get_content(msg);
		int index = online_list.indexOf(temp_id);
		if (index >=0 && temp_id != id) {
			delete_member(temp_id, index);
			online_list.remove(index);
		}
		return;
	}

	if (get_fun(msg) == "list") {
		QString content = get_content(msg);
		//ȥ�������ߵ�
		for (int i = 0; i < online_list.length(); i++)
		{
			if (content.indexOf(online_list[i]) == -1) {
				delete_member(online_list[i], i);
				online_list.remove(i);
			}
		}

		//��������ߵ�
		int index;
		QString temp_id;
		while (!content.isEmpty())
		{
			index = content.indexOf(';');
			temp_id = content.left(index);

			if (online_list.indexOf(temp_id) == -1&& temp_id !=id){
				online_list.append(temp_id);
				add_member(temp_id);
			}
			content = content.remove(0, index + 1);
		}
		return;
	}
}

void App::on_member(QString msg)
{
	emit(to_msg_client(msg));
	if (get_fun(msg) == "signal") {
		msg = get_content(msg);
		QString temp_id = get_fun(msg);
		int index = online_list.indexOf(temp_id);
		QString content = get_content(msg);
		if (content == "ask_view") {
			noti_text->append("�� " + temp_id + " ���Ͳ鿴��������");
			return;
		}

		if (content == "ask_control") {
			noti_text->append("�� " + temp_id + " ���Ϳ�����������");
		}

		if (content == "push_view") {
			noti_text->append("�� " + temp_id + " ��������鿴");
			return;
		}

		if (content == "push_control") {
			noti_text->append("�� " + temp_id + " �����������");
			return;
		}

		if (content == "close_view") {
			noti_text->append("�ر� " + temp_id + " ������鿴");
			return;
		}

		if (content == "close_control") {
			noti_text->append("�ر� " + temp_id + " ���������");
			return;
		}
	}
}


void App::handleTimeout()
{
	if (count == 0 && timer->isActive()) {
		timer->stop();
		timer->start(500);
		emit(to_msg_client("getlist;"));
		count++;
	}

	if (count == 1 && timer->isActive()) {
		timer->stop();
		timer->start(4000);
		emit(to_msg_client("getlist;"));
		count++;
	}

	if (count ==2 && timer->isActive()) {
		timer->stop();
		timer->start(5000);
		emit(to_msg_client("getlist;"));
		count++;
	}

	if (count == 3 && timer->isActive()) {
		timer->stop();
		timer->start(6000);
		emit(to_msg_client("getlist;"));
		count++;
	}

	if (count == 4 && timer->isActive()) {
		timer->stop();
		timer->start(7000);
		emit(to_msg_client("getlist;"));
		count++;
	}
}