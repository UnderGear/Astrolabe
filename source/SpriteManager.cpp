#include <algorithm>

#include "Display/SpriteManager.hpp"

SpriteManager::SpriteManager(void* TileMemoryAddress)
{
    SpriteBlock = std::unique_ptr<DoubleTileBlockRaw>
    {
        new(TileMemoryAddress) DoubleTileBlockRaw{ }
    };
    SpriteBlockIterator = SpriteBlock->begin();

    OAMMemory = reinterpret_cast<OAMT*>(OAM_ADDRESS);
    OAMAffineMemory = reinterpret_cast<OAMAffineT*>(OAM_ADDRESS);

    //TODO: do this with an algo
    for (std::int32_t i{ MaxOAMs - 1 }; i >= 0; --i)
    {
        AvailableObjectAttributes.push(&ObjectBuffer[i]);
    }
}

void SpriteManager::Tick()
{
    // TODO: only copy as much as needed?
    *OAMMemory = ObjectBuffer;
}

std::vector<SpriteManager::TileGap>::iterator SpriteManager::FindSuitableGap(std::int32_t Size)
{
    return std::ranges::find_if(Gaps, [Size](const auto& Gap)
    {
        return Gap.Length >= Size;
    });
}

void SpriteManager::MergeGaps()
{
    std::ranges::sort(Gaps, std::ranges::less{}, &TileGap::BeginIndex);
    
    // for each gap, while the next gap's start is immediately after this gap's end, extend this gap into that one, remove that one.
    auto CurrentGap{ Gaps.begin() };
    auto NextGap{ std::ranges::next(CurrentGap) };

    while (CurrentGap != Gaps.end() && NextGap != Gaps.end())
    {
        auto LastIndex{ CurrentGap->BeginIndex + CurrentGap->Length };
        if (LastIndex + 1 == NextGap->BeginIndex)
        {
            // Roll next gap into this one
            CurrentGap->Length += NextGap->Length;
            Gaps.erase(NextGap);
            
            // Remove next gap, advance the iterator, check again
            NextGap = std::ranges::next(CurrentGap);
            continue;
        }
        
        CurrentGap = NextGap;
        NextGap = std::ranges::next(CurrentGap);
    }
}

std::int32_t SpriteManager::AddToPalette(const PaletteBankAsset& PaletteAsset)
{
    return SpritePaletteManager.AddToPalette(PaletteAsset);
}

void SpriteManager::RemoveFromPalette(std::int32_t Index)
{
    SpritePaletteManager.RemoveFromPalette(Index);
}

std::int32_t SpriteManager::LoadTiles(const SpriteTileAsset& ToAdd)
{
    // If it's already loaded, increment the reference count and return the index
    auto Found{ std::ranges::find(LoadedTiles, ToAdd.ID, &SpriteTileData::AssetID) };
    if (Found != LoadedTiles.end())
    {
        Found->ReferenceCount++;
        return Found->GetTileIndex();
    }

    // Look through the gap list for a spot that can fit it.
    auto ToAddSize{ ToAdd.Data.size() };
    auto FoundGap{ FindSuitableGap(ToAddSize) };
    if (FoundGap != Gaps.end())
    {
        auto BeginIndex{ FoundGap->BeginIndex };

        // Copy the data into vram
        std::ranges::copy(ToAdd.Data, std::ranges::next(SpriteBlock->begin(), BeginIndex));

        // Shrink the gap
        auto ShouldRemove{ FoundGap->Shrink(ToAddSize) };
        if (ShouldRemove)
        {
            Gaps.erase(FoundGap);
            MergeGaps();
        }

        // Add bookkeeping
        auto& Added{ LoadedTiles.emplace_back(1, ToAdd.ID, BeginIndex, ToAddSize) };
        return Added.GetTileIndex();
    }

    // Check that it'll fit at the end of our vram
    if (std::distance(SpriteBlockIterator, SpriteBlock->end()) < static_cast<std::int32_t>(ToAddSize))
        return {};

    // Copy the data into vram
    auto [LastCopied, Next]{ std::ranges::copy(ToAdd.Data, SpriteBlockIterator) };

    // Advance the tail iterator
    auto BeginIndex{ std::distance(SpriteBlock->begin(), SpriteBlockIterator) };
    SpriteBlockIterator = Next;

    // Add bookkeeping
    auto& Added{ LoadedTiles.emplace_back(1, ToAdd.ID, BeginIndex, ToAddSize) };
    return Added.GetTileIndex();
}

void SpriteManager::UnloadTiles(std::int32_t Index)
{
    // Check to see that the asset is actually loaded
    auto Found{ std::ranges::find(LoadedTiles, Index, &SpriteTileData::GetTileIndex) };
    if (Found == LoadedTiles.end())
        return;

    // Decrement reference count, bail if we still need it loaded
    Found->ReferenceCount--;
    if (Found->ReferenceCount > 0)
        return;

    // TODO: Do we even need to do this fill?
    // Clear associated blocks of VRAM
    auto Begin{ std::ranges::next(SpriteBlock->begin(), Found->BeginIndex) };
    auto End{ std::ranges::next(SpriteBlock->begin(), Found->BeginIndex + Found->Length) };
    std::ranges::fill(Begin, End, 0);

    // Add a gap where we just removed
    Gaps.emplace_back(Found->BeginIndex, Found->Length);

    // We may have added a gap adjacent to other gaps. Combine them.
    MergeGaps();

    //TODO: should this go into MergeGaps?
    // If there's a gap before our iterator, wipe out the gap and move the iterator back to the gap's start
    auto LastGapIterator{ std::ranges::prev(Gaps.end()) };
    if (LastGapIterator != Gaps.begin())
    {
        auto LastIterator{ std::ranges::next(SpriteBlock->begin(), LastGapIterator->BeginIndex + LastGapIterator->Length) };
        if (std::ranges::next(LastIterator) == SpriteBlockIterator)
        {
            SpriteBlockIterator = std::ranges::next(SpriteBlock->begin(), LastGapIterator->BeginIndex);
            Gaps.erase(LastGapIterator);
        }
    }

    // Remove our bookkeeping for this unloaded asset
    LoadedTiles.erase(Found);
}

ObjectAttributes* SpriteManager::RequestOAM()
{
    if (AvailableObjectAttributes.size() == 0)
        return nullptr;

    ObjectAttributes* Result{ AvailableObjectAttributes.top() };
    AvailableObjectAttributes.pop();
    return Result;
}

void SpriteManager::ReleaseOAM(ObjectAttributes& OAM)
{
    //TODO: do we need to clear out any of the other stuff? we definitely DON'T want to touch the padding section
    // Hide the object
    OAM.Attribute0.ObjectMode = static_cast<std::uint16_t>(Attribute0ObjectMode::Hidden);
    AvailableObjectAttributes.push(&OAM);
}
