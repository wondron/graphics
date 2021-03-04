/*!
 ***************************************************************************
 * Copyright (C) 2018 The Sunny Company Std.
 *
 * \file       cremoteexecute.h
 * \brief      远程线程安全调用
 * \details
 *
 * \author     wengzhuohuang  <hzweng@sunnyoptical.com>
 * \version    v1.0.0
 * \date       2018-04-01
 ***************************************************************************
 * revision:
 *
 ***************************************************************************
 */

#pragma once
#include <QObject>
//#include "async_global.h"
#include "cfunction.h"

namespace Async {

class  CExecute
{
public:
    using Func = Utils::CFunction<void ()>;
    explicit CExecute(bool block = false);
    explicit CExecute(QObject* target, bool block = false);

    inline bool isBlock() const;
    inline void setBlock(bool block);

    inline QObject* target() const;
    inline void setTarget(QObject* target);

    void run(const Func& func) const;
    inline void operator <= (const Func& func) const;
private:
    QObject* m_target;
    bool     m_block;
};

bool CExecute::isBlock() const {return m_block;}

void CExecute::setBlock(bool block) {m_block = block;}

QObject* CExecute::target() const {return m_target;}

void CExecute::setTarget(QObject* target) {m_target = target;}

void CExecute::operator <=(const CExecute::Func &func) const{run(func);}

}
