#include "pqDualComboBox.h"

#include "pqPropertiesPanel.h"

#include "vtkSMEnumerationDomain.h"
#include "vtkSMProperty.h"
#include "vtkSMPropertyHelper.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QtDebug>

class pqDualComboBox::pqInternals {
 public:
  QList<QComboBox*> ComboBoxes;
  pqInternals(vtkSMProperty* smproperty, pqDualComboBox* self) {
    QVBoxLayout* vbox = new QVBoxLayout(self);
    vbox->setSpacing(pqPropertiesPanel::suggestedVerticalSpacing());
    vbox->setMargin(pqPropertiesPanel::suggestedMargin());

    for (unsigned int cc = 0, max = vtkSMPropertyHelper(smproperty)
                                     .GetNumberOfElements();
         cc < max; cc++) {
      QComboBox* box = new QComboBox(self);
      vbox->addWidget(box);
      this->ComboBoxes.push_back(box);

      // this ensures that the combo-box is filled with the values from
      // enumeration domain.
      this->fillDomainValues(
       box, vtkSMEnumerationDomain::SafeDownCast(smproperty->GetDomain(
             QString("enum_%1").arg(cc).toLatin1().data())));
    }
  }

  void fillDomainValues(QComboBox* box, vtkSMEnumerationDomain* domain) {
    for (unsigned int cc = 0, max = domain->GetNumberOfEntries(); cc < max;
         cc++) {
      box->addItem(domain->GetEntryText(cc), domain->GetEntryValue(cc));
    }
  }
};

//-----------------------------------------------------------------------------
pqDualComboBox::pqDualComboBox(vtkSMProxy* smproxy, vtkSMProperty* smproperty,
                               QWidget* parentObject)
 : Superclass(smproxy, parentObject)
 , Internals(new pqDualComboBox::pqInternals(smproperty, this)) {
  for (int cc = 0, max = this->Internals->ComboBoxes.size(); cc < max; cc++) {
    // sets up a connection between the smproperty and the QComboBox.
    this->addPropertyLink(this->Internals->ComboBoxes[cc], "currentText",
                          SIGNAL(currentIndexChanged(const QString&)), smproxy,
                          smproperty, cc);
  }
}

//-----------------------------------------------------------------------------
pqDualComboBox::~pqDualComboBox() {}
