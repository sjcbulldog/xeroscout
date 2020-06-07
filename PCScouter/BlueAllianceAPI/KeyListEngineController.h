//
// Copyright 2020 by Jack W. (Butch) Griffin
//
// Courtesy of Error Code Xero
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
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
