#if !defined(INC_H___AF68359F_BDCC_4C1A_956E_C4CA789E5014__HEAD__)
#define INC_H___AF68359F_BDCC_4C1A_956E_C4CA789E5014__HEAD__


namespace shared
{
	namespace otl {
		class @tablename {
		public:
		@declare

			@tablename() {
			}

			@tablename(const @tablename& model)
			{
				@initial
			}

			// assignment operator
			@tablename& operator=(const @tablename& model)
			{
				@equal
				return *this;
			}
		};

		// redefined operator>> for reading SchoolBusConfigSettingHistory& from otl_stream
		otl_stream& operator>>(otl_stream& s, @tablename& model) {
			@right
			return s;
		}

		otl_stream& operator<<(otl_stream& s, const @tablename& model) {
			@left
			return s;
		}

		template <typename T>
		class @tablenameObj : public ObjBase<T> {
		public:

			static @tablenameObj GetInstance() {
				static @tablenameObj instance;
				return instance;
			}

			void GetModel(T& model) {
				ObjBase<T>::GetModel(model);
			}

			void GetModelList(std::vector<T>& models) {
				ObjBase<T>::GetModelList(models);
			}

			void GetModelList(SqlServerSqlWhere& sqlWhere, std::vector<T>& models) {
				ObjBase<T>::GetModelList(sqlWhere, models);
			}

			void GetModelList(const std::string& sqlWhere, std::vector<T>& models) {
				ObjBase<T>::GetModelList(sqlWhere, models);
			}

			void Add(T& model) {
				std::string sql = R"(@add)";
				ObjBase<T>::Add(sk::Log::Format(sql, __super::m_odbc_tablename), model);
			}

			void DeleteSingle(int id) {
				ObjBase<T>::DeleteSingle(__super::m_odbc_tablename, id);
			}

			void Delete(SqlServerSqlWhere& sqlWhere) {
				ObjBase<T>::Delete(__super::m_odbc_tablename, sqlWhere);
			}

			void UpdateSingle(T& model) {
				std::string sql = R"(@update)";
				ObjBase<T>::UpdateSingle(sk::Log::Format(sql, __super::m_odbc_tablename), model);
			}
		};
	}///namespace otl
}///namespace shared

/// /*新生®（上海）**/
/// /*2022_10_04T22:53:15.4322170Z**/
/// /*_ _ _ _ _ _ _ www.skstu.com _ _ _ _ _ _ _**/
#endif ///INC_H___AF68359F_BDCC_4C1A_956E_C4CA789E5014__HEAD__
