#include <table_controller.h>

#include <QTableView>
#include <QAbstractItemModel>
#include <QEvent>
#include <QPushButton>
#include <QRect>
#include <QHeaderView>
#include <QTimer>
#include <QCoreApplication>
#include <QShowEvent>
#include <QDebug>

namespace
{

constexpr int ADD_ROW_BUTTON_WIDT{ 50 };
constexpr int ADD_ROW_BUTTON_HEIGHT{ 50 };

}

namespace tables_utils
{

class TableController::PrivateData
{
public:

	PrivateData(QTableView* pView_)
		: pView(pView_)
		, pModel(pView->model())
		, pAddRowButton(new QPushButton{ pView })
	{
		initAddRowButton();
	}

	QTableView* const pView{ nullptr };
	QAbstractItemModel* const pModel{ nullptr };
	QPushButton* const pAddRowButton{ nullptr };

	void initAddRowButton();
	void setAddRowBttnGeometry();
};

void TableController::PrivateData::initAddRowButton()
{
	//Set text and size
	{
		pAddRowButton->setText("Add row");
		pAddRowButton->setFixedSize(ADD_ROW_BUTTON_WIDT, ADD_ROW_BUTTON_HEIGHT);
	}

	//Set geometry
	{
		setAddRowBttnGeometry();
	}
}

void TableController::PrivateData::setAddRowBttnGeometry()
{
	const QRect viewportGeometry{ pView->viewport()->geometry() };
	const auto headersHeight{ pView->horizontalHeader()->height() };
	const auto rowCount{ pModel->rowCount() };
	auto yPos{ viewportGeometry.y() + headersHeight };
	if (rowCount > 0) {
		yPos += pView->rowViewportPosition(rowCount - 1) /* + pView->rowHeight(rowCount - 1)*/;
	}
	qDebug() << yPos;
	const QRect buttonGeometry{ viewportGeometry.x(), yPos, pAddRowButton->width(), pAddRowButton->height() };
	pAddRowButton->setGeometry(buttonGeometry);
}

TableController::TableController(QTableView* pView, QObject* pParent)
	: QObject(pParent)
	, m_pData(std::make_unique<PrivateData>(pView))
{
	m_pData->pView->installEventFilter(this);

	//Connections
	{
		connect(m_pData->pAddRowButton, &QPushButton::clicked, this, [this]() {
			const auto currentRowCount{ m_pData->pModel->rowCount() };
			m_pData->pModel->insertRows(currentRowCount, 1, QModelIndex{});
	     });

		connect(m_pData->pModel, &QAbstractItemModel::rowsInserted, this, [this]() {
			m_pData->setAddRowBttnGeometry();
	    });
	}
}

TableController::~TableController() = default;

bool TableController::eventFilter(QObject* pReceiver, QEvent* pEvent)
{
	return QObject::eventFilter(pReceiver, pEvent);
}

} // namespace tables_utils