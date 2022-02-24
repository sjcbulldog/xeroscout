//
// Copyright(C) 2020 by Jack (Butch) Griffin
//
//	This program is free software : you can redistribute it and /or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.If not, see < https://www.gnu.org/licenses/>.
//
//
//
// This work we create by the named individual(s) above in support of the
// FRC robotics team Error Code Xero.
//

#pragma once

#include "EngineController.h"
#include "BACountResult.h"
#include <QStringList>
#include <memory>

namespace xero
{
	namespace ba
	{
		class KeyListEngineController : public EngineController
		{
		public:
			KeyListEngineController(BlueAllianceEngine& engine, const QStringList &keys) : EngineController(engine) {
				keys_ = keys;
				index_ = 0;
			}

			virtual ~KeyListEngineController()	{
			}


			QString name() override {
				return "KeyListEngine";
			}


			std::shared_ptr<BlueAllianceResult> finishedResult()
			{
				return std::make_shared<BACountResult>(size(), size(), true);
			}

			std::shared_ptr<BlueAllianceResult> startResult()
			{
				return std::make_shared<BACountResult>(0, 0, false);
			}

			std::shared_ptr<BlueAllianceResult> progressResult()
			{
				return std::make_shared<BACountResult>(index(), size(), true);
			}

			BlueAllianceResult::Status  processDocNotFound()
			{
				nextKey();
				return BlueAllianceResult::Status::Success;
			}

			bool isDone() override {
				return index_ == keys_.size();
			}

		protected:
			const QString& getKey() const {
				return keys_[index_];
			}

			void nextKey() {
				index_++;
			}

			int size() const {
				return keys_.size();
			}

			int index() const {
				return index_;
			}
		private:
			QStringList keys_;
			int index_;
		};
	}
}
