// rdrdcreateauxfieldstable.cpp
//
// Create a new, empty Rivendell log table.
//
//   (C) Copyright 2002-2003,2016-2017 Fred Gleason <fredg@paravelsystems.com>
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

#include <rddb.h>

void RDCreateAuxFieldsTable(QString keyname,RDConfig *config)
{
  keyname.replace(" ","_");
  QString sql=QString("create table if not exists `")+
    keyname+"_FIELDS` (CAST_ID int unsigned not null primary key) "+
    config->createTablePostfix();
  RDSqlQuery *q=new RDSqlQuery(sql);
  delete q;
}

