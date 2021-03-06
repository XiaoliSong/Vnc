#pragma once
#include "head.h"
#include "member.h"
#include "parse.h"
#include <map>

//������
class App : public QDialog
{
	Q_OBJECT
private:
	QString id;
	QString pw;
	QString web_url;
	QString vnc_web_url;

	bool is_run = false;
	int count = 0;
	QVector<QString> online_list;
	QString to_public_file_url;
	QString to_person_fie_url;
	QString to_change_info_url;
	
public:
	App(QWidget *parent);
	~App() {};
	QTimer *timer;
	QPushButton *to_run_stop_btn;
	QPushButton *to_public_file_btn;
	QPushButton *to_person_file_btn;
	QPushButton *to_change_info_btn;
	QTableWidget *online_table;
	QTextEdit  *noti_text;
	QComboBox *view_cbo_sex;
	QComboBox *control_cbo_sex;

	void add_member(QString id);
	void delete_member(QString id, int i);
	void set_info(QString id, QString pw,QString vnc_url);
	void to_public_file();
	void to_person_file();
	void to_change_info();
	void to_run_stop();
signals:
	void run_stop(bool is_run);
	void to_msg_client(QString msg);
public slots:
	void on_control_cbo_sex_index_actived();
	void on_msg_client(QString msg);
	void on_vnc_app(QString msg);
	void on_member(QString msg);
	void handleTimeout();
};
