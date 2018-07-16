// edit_dropbox.cpp
//
// Edit a Rivendell Dropbox Configuration
//
//   (C) Copyright 2002-2007,2016-2018 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#include <math.h>

#include <qdialog.h>
#include <qstring.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include <qtextedit.h>
#include <qpainter.h>
#include <qevent.h>
#include <qmessagebox.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qvalidator.h>
#include <qfiledialog.h>

#include <rdapplication.h>
#include <rdcart_dialog.h>
#include <rddb.h>
#include <rdescape_string.h>

#include "globals.h"
#include "edit_dropbox.h"

EditDropbox::EditDropbox(int id,QWidget *parent)
  : QDialog(parent,"",true)
{
  QString sql;
  RDSqlQuery *q;

  //
  // Fix the Window Size
  //
  setMinimumWidth(sizeHint().width());
  setMaximumWidth(sizeHint().width());
  setMinimumHeight(sizeHint().height());
  setMaximumHeight(sizeHint().height());

  box_dropbox=new RDDropbox(id);

  setCaption(tr("Dropbox Configuration")+" ["+
	     tr("ID")+QString().sprintf(": %d]",id));

  //
  // Create Fonts
  //
  QFont normal_font=QFont("Helvetica",12,QFont::Normal);
  normal_font.setPixelSize(12);
  QFont font=QFont("Helvetica",12,QFont::Bold);
  font.setPixelSize(12);

  //
  // Dialogs
  //
  box_schedcodes_dialog=new RDSchedCodesDialog(this);

  //
  // Group Name
  //
  box_group_name_box=new QComboBox(this);
  box_group_name_box->setGeometry(120,10,100,20);
  QLabel *label=new QLabel(box_group_name_box,tr("Default Group:"),this);
  label->setGeometry(10,10,105,20);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Path
  //
  box_path_edit=new QLineEdit(this);
  box_path_edit->setGeometry(120,32,sizeHint().width()-190,19);
  box_path_edit->setMaxLength(255);
  label=new QLabel(box_path_edit,tr("&Path Spec:"),this);
  label->setGeometry(10,32,105,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  QPushButton *button=
    new QPushButton(tr("Select"),this);
  button->setGeometry(sizeHint().width()-60,30,50,23);
  button->setFont(normal_font);
  connect(button,SIGNAL(clicked()),this,SLOT(selectPathData()));

  //
  // To Cart
  //
  box_to_cart_edit=new QLineEdit(this);
  box_to_cart_edit->setGeometry(120,54,60,19);
  box_to_cart_edit->setValidator(new QIntValidator(1,999999,this));
  box_to_cart_edit->setMaxLength(6);
  label=new QLabel(box_to_cart_edit,tr("To &Cart:"),this);
  label->setGeometry(10,54,105,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  box_select_cart_button=
    new QPushButton(tr("Select"),this);
  box_select_cart_button->setGeometry(190,52,50,23);
  box_select_cart_button->setFont(normal_font);
  connect(box_select_cart_button,SIGNAL(clicked()),
	  this,SLOT(selectCartData()));

  //
  // Delete Cuts
  //
  box_delete_cuts_box=new QCheckBox(this);
  box_delete_cuts_box->setGeometry(260,56,15,15);
  box_delete_cuts_label=
    new QLabel(box_delete_cuts_box,tr("Delete cuts before importing"),this);
  box_delete_cuts_label->setGeometry(280,54,sizeHint().width()-150,20);
  box_delete_cuts_label->setFont(font);
  box_delete_cuts_label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Metadata Format
  //
  box_metadata_pattern_edit=new QLineEdit(this);
  box_metadata_pattern_edit->setGeometry(120,76,sizeHint().width()-170,19);
  box_metadata_pattern_edit->setMaxLength(64);
  label=new QLabel(box_metadata_pattern_edit,tr("&Metadata Pattern:"),this);
  label->setGeometry(10,76,105,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // User Defined Field
  //
  box_user_defined_edit=new QLineEdit(this);
  box_user_defined_edit->setGeometry(120,98,sizeHint().width()-170,19);
  box_user_defined_edit->setMaxLength(255);
  label=new QLabel(box_user_defined_edit,tr("&User Defined:"),this);
  label->setGeometry(10,98,105,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);

  //
  // Log Path
  //
  box_log_path_edit=new QLineEdit(this);
  box_log_path_edit->setGeometry(120,120,sizeHint().width()-190,19);
  box_log_path_edit->setMaxLength(255);
  label=
    new QLabel(box_log_path_edit,tr("&Log File:"),this);
  label->setGeometry(10,120,105,19);
  label->setFont(font);
  label->setAlignment(AlignRight|AlignVCenter|ShowPrefix);
  button=new QPushButton(tr("Select"),this);
  button->setGeometry(sizeHint().width()-60,118,50,23);
  button->setFont(normal_font);
  connect(button,SIGNAL(clicked()),this,SLOT(selectLogPathData()));

  //
  // Scheduler Codes
  //
  box_schedcodes_button=new QPushButton(tr("Scheduler Codes"),this);
  box_schedcodes_button->setGeometry(110,145,200,25);
  box_schedcodes_button->setFont(font);
  connect(box_schedcodes_button,SIGNAL(clicked()),this,SLOT(schedcodesData()));

  //
  // Delete Source
  //
  box_delete_source_box=new QCheckBox(this);
  box_delete_source_box->setGeometry(90,177,15,15);
  label=new QLabel(box_delete_source_box,tr("Delete source files after import"),
		   this);
  label->setGeometry(110,175,sizeHint().width()-120,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Force To Mono
  //
  box_force_to_mono_box=new QCheckBox(this);
  box_force_to_mono_box->setGeometry(90,199,15,15);
  label=new QLabel(box_force_to_mono_box,tr("Force to Monaural"),this);
  label->setGeometry(110,197,sizeHint().width()-120,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Normalization
  //
  box_normalization_box=new QCheckBox(this);
  box_normalization_box->setGeometry(90,221,15,15);
  label=new QLabel(box_normalization_box,tr("Normalize Levels"),this);
  label->setGeometry(110,219,100,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  box_normalization_level_spin=new QSpinBox(this);
  box_normalization_level_spin->setGeometry(275,219,50,20);
  box_normalization_level_spin->setRange(-100,-1);
  box_normalization_level_label=new QLabel(tr("Level:"),this);
  box_normalization_level_label->setGeometry(210,219,60,20);
  box_normalization_level_label->setFont(font);
  box_normalization_level_label->setAlignment(AlignVCenter|AlignRight);
  box_normalization_level_unit=new QLabel(tr("dBFS"),this);
  box_normalization_level_unit->setGeometry(330,219,60,20);
  box_normalization_level_unit->setAlignment(AlignVCenter|AlignLeft);
  connect(box_normalization_box,SIGNAL(toggled(bool)),
	  this,SLOT(normalizationToggledData(bool)));

  //
  // Autotrim
  //
  box_autotrim_box=new QCheckBox(this);
  box_autotrim_box->setGeometry(90,245,15,15);
  label=new QLabel(box_autotrim_box,tr("Autotrim Cuts"),this);
  label->setGeometry(110,243,100,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  box_autotrim_level_spin=new QSpinBox(this);
  box_autotrim_level_spin->setGeometry(275,243,50,20);
  box_autotrim_level_spin->setRange(-100,-1);
  box_autotrim_level_label=new QLabel(tr("Level:"),this);
  box_autotrim_level_label->setGeometry(210,243,60,20);
  box_autotrim_level_label->setFont(font);
  box_autotrim_level_label->setAlignment(AlignVCenter|AlignRight);
  box_autotrim_level_unit=new QLabel(tr("dBFS"),this);
  box_autotrim_level_unit->setGeometry(330,243,60,20);
  box_autotrim_level_unit->setAlignment(AlignVCenter|AlignLeft);
  connect(box_autotrim_box,SIGNAL(toggled(bool)),
	  this,SLOT(autotrimToggledData(bool)));

  //
  // Segue
  //
  box_segue_box=new QCheckBox(this);
  box_segue_box->setGeometry(90,271,15,15);
  label=new QLabel(box_segue_box,tr("Insert Segue Markers"),
     this);
  label->setGeometry(110,269,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  box_segue_level_spin=
    new QSpinBox(this);
  box_segue_level_spin->setGeometry(300,295,50,20);
  box_segue_level_spin->setRange(-100,0);
  box_segue_level_label=
    new QLabel(box_segue_level_spin,tr("Segue Level:"),
        this);
  box_segue_level_label->setGeometry(120,295,160,20);
  box_segue_level_label->setFont(font);
  box_segue_level_label->setAlignment(AlignVCenter|AlignRight);
  box_segue_level_unit=
    new QLabel(box_segue_level_spin,("dBFS"),this);
  box_segue_level_unit->setGeometry(360,296,60,20);
  box_segue_level_unit->setAlignment(AlignVCenter|AlignLeft);

  box_segue_length_spin=new QSpinBox(this);
  box_segue_length_spin->setGeometry(300,320,70,20);
  box_segue_length_spin->setRange(0,180000);
  box_segue_length_label=
     new QLabel(box_segue_length_spin,tr("Segue Length:"),
      this);
  box_segue_length_label->setGeometry(120,320,160,20);
  box_segue_length_label->setFont(font);
  box_segue_length_label->setAlignment(AlignVCenter|AlignRight);
  box_segue_length_unit=
    new QLabel(box_segue_length_spin,("msec"),this);
  box_segue_length_unit->setGeometry(375,321,60,20);
  box_segue_length_unit->setAlignment(AlignVCenter|AlignLeft);
  connect(box_segue_box,SIGNAL(toggled(bool)),
          this,SLOT(segueToggledData(bool)));

  //
  // Use CartChunk ID
  //
  box_use_cartchunk_id_box=new QCheckBox(this);
  box_use_cartchunk_id_box->setGeometry(90,350,15,15);
  label=new QLabel(box_use_cartchunk_id_box,
		   tr("Get cart number from CartChunk CutID"),this);
  label->setGeometry(110,348,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Title from CartChunk ID
  //
  box_title_from_cartchunk_id_box=new QCheckBox(this);
  box_title_from_cartchunk_id_box->setGeometry(90,374,15,15);
  label=new QLabel(box_title_from_cartchunk_id_box,
		   tr("Get cart title from CartChunk CutID"),this);
  label->setGeometry(110,372,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Fix Broken Formats
  //
  box_fix_broken_formats_box=new QCheckBox(this);
  box_fix_broken_formats_box->setGeometry(90,398,15,15);
  label=new QLabel(box_fix_broken_formats_box,
		   tr("Attempt to work around malformatted input files"),this);
  label->setGeometry(110,396,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Start Date Offset
  //
  box_startoffset_spin=
    new QSpinBox(this);
  box_startoffset_spin->setGeometry(215,422,50,20);
  box_startoffset_spin->setRange(-7,7);
  label=new QLabel(box_startoffset_spin,tr("Offset start date by"),this);
  label->setGeometry(90,422,120,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  label=new QLabel(box_startoffset_spin,tr("days"),this);
  label->setGeometry(275,424,100,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // End Date Offset
  //
  box_endoffset_spin=
    new QSpinBox(this);
  box_endoffset_spin->setGeometry(215,446,50,20);
  box_endoffset_spin->setRange(-7,7);
  label=new QLabel(box_endoffset_spin,tr("Offset end date by"),this);
  label->setGeometry(90,446,120,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  label=new QLabel(box_endoffset_spin,tr("days"),this);
  label->setGeometry(275,446,100,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);

  //
  // Create Dates
  //
  box_create_dates_box=new QCheckBox(this);
  box_create_dates_box->setGeometry(90,470,15,15);
  label=new QLabel(box_create_dates_box,tr("Create Dates when no Dates Exist"),
		   this);
  label->setGeometry(110,468,sizeHint().width()-40,20);
  label->setFont(font);
  label->setAlignment(AlignLeft|AlignVCenter|ShowPrefix);
  box_create_startdate_offset_spin=
    new QSpinBox(this);
  box_create_startdate_offset_spin->setGeometry(300,494,50,20);
  box_create_startdate_offset_spin->setRange(-180,180);
  box_create_startdate_label= 
    new QLabel(box_create_startdate_offset_spin,tr("Create start date offset:"),
	       this);
  box_create_startdate_label->setGeometry(120,494,160,20);
  box_create_startdate_label->setFont(font);
  box_create_startdate_label->setAlignment(AlignVCenter|AlignRight);
  box_create_startdate_unit=
    new QLabel(box_create_startdate_offset_spin,("days"),this);
  box_create_startdate_unit->setGeometry(360,495,60,20);
  box_create_startdate_unit->setAlignment(AlignVCenter|AlignLeft);
  box_create_enddate_offset_spin=new QSpinBox(this);
  box_create_enddate_offset_spin->setGeometry(300,524,50,20);
  box_create_enddate_offset_spin->setRange(-180,180);
  box_create_enddate_label= 
     new QLabel(box_create_enddate_offset_spin,tr("Create end date offset:"),
		this);
  box_create_enddate_label->setGeometry(120,524,160,20);
  box_create_enddate_label->setFont(font);
  box_create_enddate_label->setAlignment(AlignVCenter|AlignRight);
  box_create_enddate_unit=
    new QLabel(box_create_enddate_offset_spin,("days"),this);
  box_create_enddate_unit->setGeometry(360,524,60,20);
  box_create_enddate_unit->setAlignment(AlignVCenter|AlignLeft);
  connect(box_create_dates_box,SIGNAL(toggled(bool)),
          this,SLOT(createDatesToggledData(bool)));


  //
  //  Reset Button
  //
  QPushButton *reset_button=new QPushButton(this);
  reset_button->
    setGeometry(10,sizeHint().height()-60,80,50);
  reset_button->setDefault(true);
  reset_button->setFont(font);
  reset_button->setText(tr("&Reset"));
  connect(reset_button,SIGNAL(clicked()),this,SLOT(resetData()));

  //
  //  Ok Button
  //
  QPushButton *ok_button=new QPushButton(this);
  ok_button->setGeometry(sizeHint().width()-180,sizeHint().height()-60,80,50);
  ok_button->setDefault(true);
  ok_button->setFont(font);
  ok_button->setText(tr("&OK"));
  connect(ok_button,SIGNAL(clicked()),this,SLOT(okData()));

  //
  //  Cancel Button
  //
  QPushButton *cancel_button=new QPushButton(this);
  cancel_button->setGeometry(sizeHint().width()-90,sizeHint().height()-60,
			     80,50);
  cancel_button->setFont(font);
  cancel_button->setText(tr("&Cancel"));
  connect(cancel_button,SIGNAL(clicked()),this,SLOT(cancelData()));

  //
  // Populate Fields
  //
  sql="select NAME from GROUPS order by NAME";
  q=new RDSqlQuery(sql);
  QString groupname=box_dropbox->groupName();
  while(q->next()) {
    box_group_name_box->insertItem(q->value(0).toString());
    if(q->value(0).toString()==groupname) {
      box_group_name_box->setCurrentItem(box_group_name_box->count()-1);
    }
  }
  delete q;
  box_path_edit->setText(box_dropbox->path());
  if(box_dropbox->toCart()>0) {
    box_to_cart_edit->setText(QString().sprintf("%06u",box_dropbox->toCart()));
  }
  box_delete_cuts_box->setChecked(box_dropbox->deleteCuts());
  box_metadata_pattern_edit->setText(box_dropbox->metadataPattern());
  box_user_defined_edit->setText(box_dropbox->userDefined());
  box_delete_source_box->setChecked(box_dropbox->deleteSource());
  box_force_to_mono_box->setChecked(box_dropbox->forceToMono());
  box_normalization_box->setChecked(box_dropbox->normalizationLevel()<0);
  box_normalization_level_spin->
    setValue(box_dropbox->normalizationLevel()/100);
  box_autotrim_box->setChecked(box_dropbox->autotrimLevel()<0);
  box_autotrim_level_spin->setValue(box_dropbox->autotrimLevel()/100);
  box_use_cartchunk_id_box->setChecked(box_dropbox->useCartchunkId());
  box_title_from_cartchunk_id_box->setChecked(box_dropbox->titleFromCartchunkId());
  box_log_path_edit->setText(box_dropbox->logPath());
  box_fix_broken_formats_box->setChecked(box_dropbox->fixBrokenFormats());
  box_startoffset_spin->setValue(box_dropbox->startdateOffset());
  box_endoffset_spin->setValue(box_dropbox->enddateOffset());
  box_create_dates_box->setChecked(box_dropbox->createDates());
  box_create_startdate_offset_spin->setValue(box_dropbox->createStartdateOffset());
  box_create_enddate_offset_spin->setValue(box_dropbox->createEnddateOffset());
  normalizationToggledData(box_normalization_box->isChecked());
  autotrimToggledData(box_autotrim_box->isChecked());
  segueToggledData(box_segue_box->isChecked());
  createDatesToggledData(box_create_dates_box->isChecked());
  segueToggledData(box_segue_box->isChecked());
  box_segue_box->setChecked(box_dropbox->segueLevel()<1);
  box_segue_level_spin->setValue(box_dropbox->segueLevel()/100);
  box_segue_length_spin->setValue(box_dropbox->segueLength());

  sql=QString("select SCHED_CODE from DROPBOX_SCHED_CODES ")+
    QString().sprintf("where DROPBOX_ID=%d",box_dropbox->id());
  q=new RDSqlQuery(sql);
  while(q->next()) {
    box_schedcodes.push_back(q->value(0).toString());
  }
  delete q;
}


QSize EditDropbox::sizeHint() const
{
  return QSize(450,644);
} 


QSizePolicy EditDropbox::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
}


void EditDropbox::selectPathData()
{
  QString path=box_path_edit->text();
  path=QFileDialog::getExistingDirectory(path,this);
  if(!path.isEmpty()) {
    box_path_edit->setText(path);
  }
}


void EditDropbox::selectCartData()
{
  int cartnum=box_to_cart_edit->text().toInt();
  admin_cart_dialog->exec(&cartnum,RDCart::Audio,NULL,0,
			  rda->user()->name(),rda->user()->password());
  if(cartnum>0) {
    box_to_cart_edit->setText(QString().sprintf("%06d",cartnum));
  }
}


void EditDropbox::selectLogPathData()
{
  QString path=box_log_path_edit->text();
  path=QFileDialog::getSaveFileName(path,QString::null,this);
  if(!path.isEmpty()) {
    box_log_path_edit->setText(path);
  }
}


void EditDropbox::schedcodesData()
{
  box_schedcodes_dialog->exec(&box_schedcodes,NULL);
}


void EditDropbox::normalizationToggledData(bool state)
{
  box_normalization_level_spin->setEnabled(state);
  box_normalization_level_label->setEnabled(state);
  box_normalization_level_unit->setEnabled(state);
}


void EditDropbox::autotrimToggledData(bool state)
{
  box_autotrim_level_spin->setEnabled(state);
  box_autotrim_level_label->setEnabled(state);
  box_autotrim_level_unit->setEnabled(state);
}

void EditDropbox::segueToggledData(bool state)
{
  box_segue_level_spin->setEnabled(state);
  box_segue_level_label->setEnabled(state);
  box_segue_level_unit->setEnabled(state);
  box_segue_length_spin->setEnabled(state);
  box_segue_length_label->setEnabled(state);
  box_segue_length_unit->setEnabled(state);
  if (!state) {
    box_segue_level_spin->setValue(1);
    box_segue_length_spin->setValue(0);
  }
}

void EditDropbox::createDatesToggledData(bool state)
{
  box_create_startdate_offset_spin->setEnabled(state);
  box_create_startdate_label->setEnabled(state);
  box_create_startdate_unit->setEnabled(state);
  box_create_enddate_offset_spin->setEnabled(state);
  box_create_enddate_label->setEnabled(state);
  box_create_enddate_unit->setEnabled(state);
  if (!state) {
    box_create_startdate_offset_spin->setValue(0);
    box_create_enddate_offset_spin->setValue(0);
  }
}


void EditDropbox::resetData()
{
  if(QMessageBox::question(this,"RDAdmin - "+tr("Reset Dropbox"),
			   tr("Resetting the dropbox will clear the list of already imported\nfiles, causing any whose files remain to be imported again.")+"\n\n"+tr("Reset the dropbox?"),
			   QMessageBox::Yes,QMessageBox::No)!=QMessageBox::Yes) {
    return;
  }
  QString sql=QString("delete from DROPBOX_PATHS where ")+
    QString().sprintf("DROPBOX_ID=%d",box_dropbox->id());
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
  QMessageBox::information(this,"RDAdmin - "+tr("Reset Dropbox"),
			   tr("The dropbox has been reset."));
}


void EditDropbox::okData()
{
  QString sql;
  RDSqlQuery *q;

  //
  //  Validate End Date Offsets
  //
  if(box_create_startdate_offset_spin->value()>
     box_create_enddate_offset_spin->value()) {
    QMessageBox::warning(this,tr("Invalid Offsets"),
    tr("The Create EndDate Offset is less than the Create Start Date Offset!"));
    return;
  }

  box_dropbox->setGroupName(box_group_name_box->currentText());
  box_dropbox->setPath(box_path_edit->text());
  if(box_to_cart_edit->text().isEmpty()) {
    box_dropbox->setToCart(0);
  }
  else {
    box_dropbox->setToCart(box_to_cart_edit->text().toUInt());
  }
  box_dropbox->setDeleteCuts(box_delete_cuts_box->isChecked());
  box_dropbox->setMetadataPattern(box_metadata_pattern_edit->text());
  box_dropbox->setUserDefined(box_user_defined_edit->text());
  box_dropbox->setDeleteSource(box_delete_source_box->isChecked());
  box_dropbox->setForceToMono(box_force_to_mono_box->isChecked());
  if(box_normalization_box->isChecked()) {
    box_dropbox->
      setNormalizationLevel(box_normalization_level_spin->value()*100);
  }
  else {
    box_dropbox->setNormalizationLevel(0);
  }
  if(box_autotrim_box->isChecked()) {
    box_dropbox->
      setAutotrimLevel(box_autotrim_level_spin->value()*100);
  }

  else {
    box_dropbox->setAutotrimLevel(0);
  }
  box_dropbox->setUseCartchunkId(box_use_cartchunk_id_box->isChecked());
  box_dropbox->setTitleFromCartchunkId(box_title_from_cartchunk_id_box->isChecked());
  box_dropbox->setLogPath(box_log_path_edit->text());
  box_dropbox->setFixBrokenFormats(box_fix_broken_formats_box->isChecked());
  box_dropbox->setStartdateOffset(box_startoffset_spin->value());
  box_dropbox->setEnddateOffset(box_endoffset_spin->value());
  box_dropbox->setCreateDates(box_create_dates_box->isChecked());
  box_dropbox->setCreateStartdateOffset(box_create_startdate_offset_spin->value());
  box_dropbox->setCreateEnddateOffset(box_create_enddate_offset_spin->value());

  if(box_segue_box->isChecked()){
    box_dropbox->setSegueLevel(box_segue_level_spin->value()*100);
    box_dropbox->setSegueLength(box_segue_length_spin->value());
  }
  else{
    box_dropbox->setSegueLevel(1);
    box_dropbox->setSegueLength(0);
  }

  sql=QString("delete from DROPBOX_SCHED_CODES where ")+
    QString().sprintf("DROPBOX_ID=%d",box_dropbox->id());
  q=new RDSqlQuery(sql);
  delete q;
  for(unsigned i=0;i<box_schedcodes.size();i++) {
    sql=QString("insert into DROPBOX_SCHED_CODES set ")+
      QString().sprintf("DROPBOX_ID=%d,",box_dropbox->id())+
      "SCHED_CODE=\""+RDEscapeString(box_schedcodes[i])+"\"";
    q=new RDSqlQuery(sql);
    delete q;
  }
  done(0);
}


void EditDropbox::cancelData()
{
  done(-1);
}


