#pragma once

#include <QStringList>
#include <QString>
#include <QColor>

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class DataSetHighlightRules
			{
			public:
				DataSetHighlightRules(QString name, QStringList&& list, QStringList &&highlight, const QString& equ, QColor fore, QColor back, const QString &desc)
				{
					name_ = name;
					fields_ = std::move(list);
					highlights_ = std::move(highlight);
					equation_ = equ;
					foreground_ = fore;
					background_ = back;
					desc_ = desc;
				}

				DataSetHighlightRules(QString name, const QStringList& list, const QStringList &highlight, const QString& equ, QColor fore, QColor back, const QString &desc)
				{
					name_ = name;
					fields_ = list;
					highlights_ = highlight;
					equation_ = equ;
					foreground_ = fore;
					background_ = back;
					desc_ = desc;
				}

				virtual ~DataSetHighlightRules()
				{
				}

				const QString& name() const {
					return name_;
				}

				void setName(const QString& name) {
					name_ = name;
				}

				const QStringList& fields() const {
					return fields_;
				}

				void setFields(QStringList&& fields) {
					fields_ = std::move(fields);
				}

				void setFields(const QStringList& fields) {
					fields_ = fields;
				}

				const QStringList& highlights() const {
					return highlights_;
				}

				void setHighlights(QStringList&& highlights) {
					highlights_ = std::move(highlights);
				}

				void setHighlights(const QStringList& highlights) {
					highlights_ = highlights;
				}

				const QString& equation() const {
					return equation_;
				}

				void setEquation(const QString& equ) {
					equation_ = equ;
				}

				QColor foreground() const {
					return foreground_;
				}

				void setForeground(QColor c) {
					foreground_ = c;
				}

				QColor background() const {
					return background_;
				}

				void setBackground(QColor c) {
					background_ = c;
				}

				const QString& descriptor() const {
					return desc_;
				}

				void setDescriptor(const QString& desc) {
					desc_ = desc;
				}

			private:
				//
				// The name of this rule
				//
				QString name_;

				//
				// This rules is applied to groups of rows where these fields are all the same
				//
				QStringList fields_;

				//
				// The set of columns to highlight within the matched set of rows if the expression is true
				//
				QStringList highlights_;

				//
				// This is the equation to evaluation.  If the result is a boolean or integer that is
				// non zero, the highlight is applied
				//
				QString equation_;

				//
				// These are the cell background color and cell text color to use to highlight
				//
				QColor background_;
				QColor foreground_;

				//
				// The descriptor for the rule, describing what failed
				//
				QString desc_;
			};
		}
	}
}


