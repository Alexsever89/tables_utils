#include <table_controller.h>

#include <QTableView>
#include <QAbstractItemModel>
#include <QEvent>
#include <QPushButton>
#include <QRect>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QString>
#include <QPoint>
#include <QItemSelectionModel>
#include <QList>

#include <algorithm>

namespace
{

constexpr auto ADD_ROW_BUTTON_WIDT{ 50 };
constexpr auto ADD_ROW_BUTTON_HEIGHT{ 50 };
constexpr auto ADD_ROW_BUTTON_STYLE_SHEET{ "QPushButton{ background-color : transparent; }" };

//Context menu strings
const QString INSERT_ROW_BEFORE{ QObject::tr("Вставить строчку выше") };
const QString INSERT_ROW_AFTER{ QObject::tr("Вставить строчку ниже") };
const QString DELETE_ROW{ QObject::tr("Удалить строчку") };

}

namespace tables_utils
{

class TableController::PrivateData
{
public:

	PrivateData(TableController* pOwner_, QTableView* pView_)
		: pOwner(pOwner_)
		, pView(pView_)
		, pModel(pView->model())
		, pAddRowButton(new QPushButton{ pView })
	{
		initAddRowButton();
	}

	TableController* const pOwner{ nullptr };
	QTableView* const pView{ nullptr };
	QAbstractItemModel* const pModel{ nullptr };
	QPushButton* const pAddRowButton{ nullptr };

	std::unique_ptr<QMenu> pContextMenu;
	QAction* pInsertRowBeforeAction{ nullptr };
	QAction* pInsertRowAfterAction{ nullptr };
	QAction* pDeleteRowAction{ nullptr };
	void initContextMenu();

	void initAddRowButton();
	void setAddRowBttnGeometry();

	void insertRow(const bool isBefore);
	void deleteRow();
};

void TableController::PrivateData::initContextMenu()
{
	pContextMenu = std::make_unique<QMenu>();
	pInsertRowBeforeAction = pContextMenu->addAction(INSERT_ROW_BEFORE);
	pInsertRowAfterAction = pContextMenu->addAction(INSERT_ROW_AFTER);
	pDeleteRowAction = pContextMenu->addAction(DELETE_ROW);

	QObject::connect(pInsertRowAfterAction, &QAction::triggered, pOwner, [this]() { insertRow(false); });
	QObject::connect(pInsertRowBeforeAction, &QAction::triggered, pOwner, [this]() { insertRow(true); });
	QObject::connect(pDeleteRowAction, &QAction::triggered, pOwner, [this]() { deleteRow(); });
}

void TableController::PrivateData::initAddRowButton()
{
	//Set text and size
	{
		pAddRowButton->setText("Add row");
		pAddRowButton->setFixedSize(ADD_ROW_BUTTON_WIDT, ADD_ROW_BUTTON_HEIGHT);
		pAddRowButton->setStyleSheet(ADD_ROW_BUTTON_STYLE_SHEET);
	}

	//Set geometry
	{
		setAddRowBttnGeometry();
	}
}

void TableController::PrivateData::setAddRowBttnGeometry()
{
	QHeaderView* const pVHeaders{ pView->verticalHeader() };
	const auto xPos{ pVHeaders->geometry().x() };
	const auto hHeadersHeight{ pView->horizontalHeader()->height() };
	auto yPos{ hHeadersHeight };
	const auto rowCount{ pModel->rowCount() };
	for (auto row{ 0 }; row < rowCount; ++row) {
		yPos += pView->rowHeight(row);
	}
	const QRect geometry{ xPos, yPos, pAddRowButton->width(), pAddRowButton->height() };
	pAddRowButton->setGeometry(geometry);
}

void TableController::PrivateData::insertRow(const bool isBefore)
{
	QItemSelectionModel* const pSelectionModel{ pView->selectionModel() };
	if (nullptr == pSelectionModel) {
		return;
	}

	const auto selectedIndexes{ pSelectionModel->selectedIndexes() };
	if (selectedIndexes.isEmpty()) {
		return;
	}

	size_t cnt{ 0 };
	auto topRow{ 0 };
	for (const QModelIndex& index : selectedIndexes)
	{
		if (0 == cnt) {
			topRow = index.row();
		}
		else {
			topRow = std::min(topRow, index.row());
		}
		++cnt;
	}

	if (topRow < 0 || topRow >= pModel->rowCount()) {
		return;
	}

	if (isBefore) {
		pModel->insertRows(topRow, 1, QModelIndex{});
	}
	else {
		pModel->insertRows(topRow + 1, 1, QModelIndex{});
	}
}

void TableController::PrivateData::deleteRow()
{
	QItemSelectionModel* const pSelectionModel{ pView->selectionModel() };
	if (nullptr == pSelectionModel) {
		return;
	}

	const auto selectedIndexes{ pSelectionModel->selectedIndexes() };
	if (selectedIndexes.isEmpty()) {
		return;
	}

	size_t cnt{ 0 };
	auto topRow{ 0 };
	for (const QModelIndex& index : selectedIndexes)
	{
		if (0 == cnt) {
			topRow = index.row();
		}
		else {
			topRow = std::min(topRow, index.row());
		}
		++cnt;
	}

	if (topRow < 0 || topRow >= pModel->rowCount()) {
		return;
	}

	pModel->removeRows(topRow, 1, QModelIndex{});
}

TableController::TableController(QTableView* pView, QObject* pParent)
	: QObject(pParent)
	, m_pData(std::make_unique<PrivateData>(this, pView))
{
	m_pData->pView->installEventFilter(this);
	m_pData->pView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

	//Connections
	{
		connect(m_pData->pAddRowButton, &QPushButton::clicked, this, [this]() {
			const auto currentRowCount{ m_pData->pModel->rowCount() };
			m_pData->pModel->insertRows(currentRowCount, 1, QModelIndex{});
	     });

		connect(m_pData->pModel, &QAbstractItemModel::rowsInserted, this, [this]() {
			m_pData->setAddRowBttnGeometry();
	    });

		connect(m_pData->pModel, &QAbstractItemModel::rowsRemoved, this, [this]() {
			m_pData->setAddRowBttnGeometry();
	    });

		//Context menu
		connect(m_pData->pView, &QWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
			if (nullptr == m_pData->pContextMenu) {
				m_pData->initContextMenu();
			}
			m_pData->pContextMenu->popup(m_pData->pView->mapToGlobal(pos));
	    });
	}
}

TableController::~TableController() = default;

bool TableController::eventFilter(QObject* pReceiver, QEvent* pEvent)
{
	return QObject::eventFilter(pReceiver, pEvent);
}

} // namespace tables_utils