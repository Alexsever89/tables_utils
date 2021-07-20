#include <table_model.h>

#include <QStringList>
#include <QVariant>
#include <QModelIndex>

#include <vector>
#include <unordered_map>

namespace
{

constexpr int INVALID_VALUE{ -1 };

}

namespace tables_utils
{

struct TableModel::PrivateData
{
public:

	PrivateData(TableModel* const pOwner_, const QStringList& headers_, const bool isHorizontal_)
		: pOwner(pOwner_)
		, isHorizontal(isHorizontal_)
	{
		vecHshRoleHeaderValue.reserve(headers_.size());
		for (const QString& header : headers_) {
			vecHshRoleHeaderValue.push_back({ { Qt::DisplayRole, header } });
		}
	}

	TableModel* const pOwner{ nullptr };
	bool isHorizontal;
	std::vector<std::vector<std::unordered_map<int, QVariant>>> data; //key - role, value - element value of role
	std::vector<std::unordered_map<int, QVariant>> vecHshRoleHeaderValue; //key - role, value - header value of role

	bool verificationData() const;
	bool isElementExist(const int row, const int column) const;
};

bool TableModel::PrivateData::verificationData() const
{
	if (vecHshRoleHeaderValue.empty()) {
		return false;
	}

	if (data.empty()) {
		return true;
	}

	size_t cnt{ 0 };
	size_t size{ 0 };
	for (const auto& row : data)
	{
		if (0 == cnt) {
			size = row.size();
		}
		else if (size != row.size()) {
			return false;
		}
		++cnt;
	}

	if (isHorizontal && (data.front().size() != vecHshRoleHeaderValue.size())) {
		return false;
	}
	else if (!isHorizontal && vecHshRoleHeaderValue.size() != data.size()) {
		return false;
	}

	return true;
}

bool TableModel::PrivateData::isElementExist(const int row, const int column) const
{
	if (row >= data.size()) {
		return false;
	}

	if (column >= data.front().size()) {
		return false;
	}

	return true;
}

TableModel::TableModel(const QStringList& columns, const bool isHorizontal, QObject* pParent)
	: QAbstractItemModel(pParent)
	, m_pData(std::make_unique<PrivateData>(this, columns, isHorizontal))
{}

TableModel::~TableModel() = default;

int TableModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	}

	if (!m_pData->verificationData()) {
		return 0;
	}

	if (m_pData->isHorizontal) {
		return m_pData->vecHshRoleHeaderValue.size();
	}

	if (m_pData->data.empty()) {
		return 0;
	}
	return m_pData->data.front().size();
}

int	TableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	}

	if (!m_pData->verificationData()) {
		return 0;
	}

	return m_pData->data.size();
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
	QVariant result;

	if (!index.isValid()) {
		return result;
	}

	if (!m_pData->verificationData()) {
		return result;
	}

	if (m_pData->data.empty()) {
		return result;
	}

	const auto row{ index.row() };
	const auto column{ index.column() };
	if (!m_pData->isElementExist(row, column)) {
		return result;
	}

	auto iterData{ m_pData->data[row][column].find(role) };
	if (m_pData->data[row][column].cend() == iterData) {
		return result;
	}
	result = iterData->second;

	return result;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant result;

	if ( (m_pData->isHorizontal && Qt::Orientation::Horizontal == orientation) || (!m_pData->isHorizontal && Qt::Orientation::Vertical == orientation) )
	{
		if (section < 0 || section >= m_pData->vecHshRoleHeaderValue.size()) {
			return result;
		}

		auto iterData{ m_pData->vecHshRoleHeaderValue[section].find(role) };
		if (m_pData->vecHshRoleHeaderValue[section].cend() == iterData) {
			return result;
		}
		result = iterData->second;
	}
	else {
		result = QAbstractItemModel::headerData(section, orientation, role);
	}

	return result;
}

QModelIndex	TableModel::index(int row, int column, const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return QModelIndex{};
	}

	return QAbstractItemModel::createIndex(row, column);
}

bool TableModel::insertColumns(int column, int count, const QModelIndex& parent)
{
	if (parent.isValid()) {
		return false;
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	beginInsertColumns(parent, column, column + count - 1);
	m_pData->vecHshRoleHeaderValue.insert(m_pData->vecHshRoleHeaderValue.cbegin() + column, count, std::unordered_map<int, QVariant>{});
	for (auto& row : m_pData->data){
		row.insert(row.cbegin() + column, count, std::unordered_map<int, QVariant>{});
	}
	endInsertColumns();

	if (!m_pData->verificationData()) {
		return false;
	}

	return true;
}

bool TableModel::insertRows(int row, int count, const QModelIndex& parent)
{
	if (parent.isValid()) {
		return false;
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	const int columns{ columnCount() };

	beginInsertRows(parent, row, row + count - 1);
	m_pData->data.insert(m_pData->data.cbegin() + row, count, std::vector<std::unordered_map<int, QVariant>>(columns));
	endInsertRows();

	if (!m_pData->verificationData()) {
		return false;
	}

	return true;
}

bool TableModel::removeColumns(int column, int count, const QModelIndex& parent)
{
	if (parent.isValid()) {
		return false;
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	beginRemoveColumns(parent, column, column + count - 1);
	for (auto& row : m_pData->data) {
		row.erase(row.cbegin() + column, row.cbegin() + column + count);
	}
	endRemoveColumns();

	if (!m_pData->verificationData()) {
		return false;
	}

	return true;
}

bool TableModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (parent.isValid()) {
		return false;
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	beginRemoveRows(parent, row, row + count - 1);
	m_pData->data.erase(m_pData->data.cbegin() + row, m_pData->data.cbegin() + row + count);
	endRemoveRows();

	if (!m_pData->verificationData()) {
		return false;
	}

	return true;
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid()) {
		return false;
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	const int row{ index.row() };
	const int column{ index.column() };
	if (!m_pData->isElementExist(row, column)) {
		return false;
	}

	m_pData->data[row][column][role] = value;
	if (Qt::EditRole == role) {
		m_pData->data[row][column][Qt::DisplayRole] = value;
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	return true;
}

bool TableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant& value, int role)
{
	if (!m_pData->verificationData()) {
		return false;
	}

	if ((m_pData->isHorizontal && Qt::Orientation::Horizontal == orientation) || (!m_pData->isHorizontal && Qt::Orientation::Vertical == orientation))
	{
		if (section < 0 || section >= m_pData->vecHshRoleHeaderValue.size()) {
			return false;
		}

		m_pData->vecHshRoleHeaderValue[section][role] = value;
	}
	else {
		return QAbstractItemModel::setHeaderData(section, orientation, value, role);
	}

	if (!m_pData->verificationData()) {
		return false;
	}

	return true;
}

QModelIndex TableModel::parent([[maybe_unused]] const QModelIndex& index) const
{
	return QModelIndex{};
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
	if (!m_pData->verificationData() || !m_pData->isElementExist(index.row(), index.column())) {
		return Qt::NoItemFlags;
	}

	return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

} // namespace tables_utils