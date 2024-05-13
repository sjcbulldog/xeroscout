#pragma once
#include "FormItemDesc.h"
#include "EditItemDisplay.h"

namespace xero
{
	namespace scouting
	{
		namespace datamodel
		{
			class EditFormItem : public FormItemDesc
			{
			public:
				EditFormItem(const QString& display, const QString& tag, int rows, int cols, int width) : FormItemDesc(display, tag)
				{
					rows_ = rows;
					cols_ = cols;
					width_ = width;
					addField(std::make_shared<FieldDesc>(tag, FieldDesc::Type::String, true));
				}

				virtual ~EditFormItem()
				{
				}

				int rows() const {
					return rows_;
				}

				int cols() const {
					return cols_;
				}

				int width() const {
					return width_;
				}

				virtual FormItemDisplay* createDisplay(ImageSupplier& images, QWidget* parent) const
				{
					return new EditItemDisplay(images, this, parent);
				}

			private:
				int rows_;
				int cols_;
				int width_;
			};
		}
	}
}
