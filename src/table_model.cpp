#include <table_model.h>

#include <QStringList>
#include <QVariant>

#include <deque>
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
		, headers(headers_)
		, isHorizontal(isHorizontal_)
	{
		vecHshRoleHeaderValue.reserve(headers.size());
		for (const QString& header : headers) {
			vecHshRoleHeaderValue.emplace_back(Qt::DisplayRole, header);
		}
	}

	TableModel* const pOwner{ nullptr };
	QStringList headers;
	bool isHorizontal;
	std::deque<std::unordered_map<int, QVariant>> data; //key - role, value - element value of role
	std::vector<std::unordered_map<int, QVariant>> vecHshRoleHeaderValue; //key - role, value - header value of role

	size_t getElementIndex(const size_t row, const size_t column);
};

size_t TableModel::PrivateData::getElementIndex(const size_t row, const size_t column)
{
	return row * pOwner->columnCount() + column;
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

	if (m_pData->isHorizontal) {
		return m_pData->headers.size();
	}

	if (m_pData->data.empty() || m_pData->headers.empty()) {
		return 0;
	}
	return m_pData->data.size() / m_pData->headers.size();
}

int	TableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	}

	if (m_pData->data.empty() || m_pData->headers.empty()) {
		return 0;
	}

	if (m_pData->isHorizontal) {
		return m_pData->data.size() / m_pData->headers.size();
	}

	return m_pData->headers.size();
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
	QVariant result;

	if (!index.isValid()) {
		return result;
	}

	const auto row{ index.row() };
	const auto column{ index.column() };
	const auto elementIndex{ m_pData->getElementIndex(row, column) };
	if (0 == elementIndex || elementIndex >= m_pData->data.size()) {
		return result;
	}

	const auto& elementData{ m_pData->data[elementIndex] };
	auto iterData{ elementData.find(role) };
	if (elementData.cend() == iterData) {
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
		if (section < 0 || section >= m_pData->headers.size() || section >= m_pData->vecHshRoleHeaderValue.size()) {
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

} // namespace tables_utils