#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>

#include <memory>

class QStringList;
class QVariant;

namespace tables_utils
{

class TableModel : public QAbstractItemModel
{
public:

	explicit TableModel(const QStringList& columns, const bool isHorizontal = true, QObject* pParent = nullptr);
	~TableModel() override;

	TableModel(const TableModel&) = delete;
	TableModel& operator=(const TableModel&) = delete;

	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex	index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex()) override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	bool setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role = Qt::EditRole) override;

private:

	struct PrivateData;
	std::unique_ptr<PrivateData> m_pData;
};

} // namespace tables_utils