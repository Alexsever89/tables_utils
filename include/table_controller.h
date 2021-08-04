#pragma once

#include <QObject>

#include <memory>

class QTableView;
class QEvent;

namespace tables_utils
{

class TableModel;

class TableController : public QObject
{
public:

	TableController(QTableView* pView, QObject* pParent = nullptr);
	~TableController() override;

protected:

	bool eventFilter(QObject* pReceiver, QEvent* pEvent) override;

private:

	class PrivateData;
	std::unique_ptr<PrivateData> m_pData;
};

} // namespace tables_utils