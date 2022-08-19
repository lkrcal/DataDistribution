// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef ALICEO2_SUBTIMEFRAME_DPL_H_
#define ALICEO2_SUBTIMEFRAME_DPL_H_

#include "SubTimeFrameDataModel.h"
#include "SubTimeFrameVisitors.h"

#include <Headers/DataHeader.h>

namespace o2::DataDistribution
{

////////////////////////////////////////////////////////////////////////////////
/// StfDplAdapter
////////////////////////////////////////////////////////////////////////////////

class StfToDplAdapter : public ISubTimeFrameVisitor
{
 public:
  StfToDplAdapter() = delete;
  StfToDplAdapter(fair::mq::Channel& pDplBridgeChan, SyncMemoryResources &pMemRes)
    : mChan(pDplBridgeChan),
      mMemRes(pMemRes)
  {
    mMessages.reserve(1 << 20);

    if (getenv("DATADIST_DEBUG_DPL_CHAN")) {
      IDDLOG("Inspection of DPL messages is enabled");
      mInspectChannel = true;
    }

    if (getenv("DATADIST_NEW_DPL_CHAN")) {
      if ("0" == std::string(getenv("DATADIST_NEW_DPL_CHAN"))) {
        mReducedHdr = false;
      }
    }

    IDDLOG("StfToDplAdapter: sending reduced-header split-payload messages={}", mReducedHdr);
  }

  virtual ~StfToDplAdapter() = default;

  void sendToDpl(std::unique_ptr<SubTimeFrame>&& pStf);

  void sendEosToDpl();

  inline void stop() { mRunning = false; }

 protected:
  void visit(SubTimeFrame& pStf, void*) override;

  void inspect() const;

 private:
  std::atomic_bool mRunning = true;
  bool mInspectChannel = false;
  bool mReducedHdr = true;

  std::vector<fair::mq::MessagePtr> mMessages;
  fair::mq::Channel& mChan;
  SyncMemoryResources& mMemRes;
};

////////////////////////////////////////////////////////////////////////////////
/// DplToStfAdapter
////////////////////////////////////////////////////////////////////////////////

class DplToStfAdapter : public ISubTimeFrameVisitor
{
  constexpr static std::uint64_t sCurrentTfId = 0;

 public:
  DplToStfAdapter() = default;
  virtual ~DplToStfAdapter() = default;

  std::unique_ptr<SubTimeFrame> deserialize(fair::mq::Channel& pChan, bool logError = false);
  std::unique_ptr<SubTimeFrame> deserialize(fair::mq::Parts& pMsgs);

 protected:
  std::unique_ptr<SubTimeFrame> deserialize_impl();
  void visit(SubTimeFrame& pStf, void*) override;

 private:
  std::vector<fair::mq::MessagePtr> mMessages;
};

} /* o2::DataDistribution */

#endif /* ALICEO2_SUBTIMEFRAME_DPL_H_ */
