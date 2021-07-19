#include <table_model.h>

#include <QStringList>
#include <QVariant>

#include <deque>
#include <unordered_map>

namespace tables_utils
{

struct TableModel::PrivateData
{
public:

	PrivateData(const QStringList& headers_, const bool isHorizontal_)
		: headers(headers_)
		, isHorizontal(isHorizontal_)
	{}

	QStringList headers;
	bool isHorizontal;
	std::deque<std::unordered_map<int, QVariant>> data;
};

TableModel::TableModel(const QStringList& columns, const bool isHorizontal, QObject* pParent)
	: QAbstractItemModel(pParent)
	, m_pData(std::make_unique<PrivateData>(columns, isHorizontal))
{}

TableModel::~TableModel() = default;

} // namespace tables_utils