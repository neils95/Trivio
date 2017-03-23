namespace Parser
{
    using System;
    using System.Collections.Generic;
    using System.ComponentModel.DataAnnotations;
    using System.ComponentModel.DataAnnotations.Schema;
    using System.Data.Entity.Spatial;

    public partial class Trivia
    {
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2214:DoNotCallOverridableMethodsInConstructors")]
        public Trivia()
        {
            UserTriviaHistories = new HashSet<UserTriviaHistory>();
        }

        public int Id { get; set; }

        [Required]
        [StringLength(500)]
        public string Text { get; set; }

        public int Upvotes { get; set; }

        public int Downvotes { get; set; }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Usage", "CA2227:CollectionPropertiesShouldBeReadOnly")]
        public virtual ICollection<UserTriviaHistory> UserTriviaHistories { get; set; }
    }
}
