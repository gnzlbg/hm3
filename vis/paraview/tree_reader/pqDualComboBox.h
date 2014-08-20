#pragma once
/// \file pqDualComboBox.h Dual Combo Box

#include "pqPropertyWidget.h"
#include <QScopedPointer>

class pqDualComboBox : public pqPropertyWidget {
  Q_OBJECT
  typedef pqPropertyWidget Superclass;

 public:
  pqDualComboBox(vtkSMProxy* smproxy, vtkSMProperty* smproperty,
                 QWidget* parent = 0);
  virtual ~pqDualComboBox();

 private:
  Q_DISABLE_COPY(pqDualComboBox);
  class pqInternals;
  QScopedPointer<pqInternals> Internals;
};
