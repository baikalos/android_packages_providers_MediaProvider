/*
 * Copyright (C) 2024 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "annot.h"

#include <utility>
#include <vector>

#include "absl/container/flat_hash_set.h"
#include "absl/types/span.h"
#include "fpdf_annot.h"

namespace pdfClient_utils {

void GetVisibleAnnotsOfType(FPDF_PAGE page, const absl::flat_hash_set<int>& types,
                            std::vector<ScopedFPDFAnnotation>* annots) {
    const int num_annots = FPDFPage_GetAnnotCount(page);
    for (int i = 0; i < num_annots; ++i) {
        ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, i));
        const FPDF_ANNOTATION_SUBTYPE subtype = FPDFAnnot_GetSubtype(annot.get());
        if (types.contains(subtype)) {
            const int annot_flags = FPDFAnnot_GetFlags(annot.get());
            // Only worry about annots that aren't already hidden.
            if ((annot_flags & FPDF_ANNOT_FLAG_HIDDEN) == FPDF_ANNOT_FLAG_NONE) {
                annots->push_back(std::move(annot));
            }
        }
    }
}

void HideAnnots(absl::Span<const ScopedFPDFAnnotation> annots) {
    for (const auto& annot : annots) {
        const int annot_flags = FPDFAnnot_GetFlags(annot.get());
        FPDFAnnot_SetFlags(annot.get(), annot_flags | FPDF_ANNOT_FLAG_HIDDEN);
    }
}

void UnhideAnnots(absl::Span<const ScopedFPDFAnnotation> annots) {
    for (const auto& annot : annots) {
        const int annot_flags = FPDFAnnot_GetFlags(annot.get());
        FPDFAnnot_SetFlags(annot.get(), annot_flags & ~FPDF_ANNOT_FLAG_HIDDEN);
    }
}

}  // namespace pdfClient_utils