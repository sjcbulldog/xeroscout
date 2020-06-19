#include "IntroView.h"
#include <QCoreApplication>
#include <QBoxLayout>


namespace xero
{
	namespace scouting
	{
		namespace views
		{
			IntroView::IntroView(const QString& name, QWidget* parent) : QWidget(parent), ViewBase(name)
			{
				QVBoxLayout* vlay = new QVBoxLayout();
				setLayout(vlay);

				browser_ = new QTextBrowser(this);
				vlay->addWidget(browser_);
			}

			IntroView::~IntroView()
			{
			}

			void IntroView::setFile(const QString& filename)
			{
				QString html = QCoreApplication::applicationDirPath() + "/html/" + filename;

				QFile file(html);
				if (file.exists())
				{
					if (file.open(QIODevice::ReadOnly))
					{
						QTextStream strm(&file);
						html = strm.readAll();
						browser_->setHtml(html);
					}
				}
			}
		}
	}
}
